// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_track.h"
#include "audio_renderer.h"
#include "error_info.h"
#include "formats.h"
#include "interval.h"
#include "media_clip.h"
#include "media_manager.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QByteArray>
#include <QDebug>
#include <QQmlInfo>
#include <Qt>
#include <chrono>
#include <cmath>
#include <ffms.h>
#include <memory>
#include <ratio>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <type_traits>
extern "C" {
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}
using namespace std::literals::string_literals;
using namespace std::chrono;
using namespace std::chrono_literals;

class Resampler {
public:
    struct ResampleContextDeleter {
        void operator()(SwrContext* c) const
        {
            swr_free(&c);
        }
    };

    explicit Resampler() = default;
    Resampler(Resampler&&) = delete;
    Resampler(const Resampler&) = delete;
    Resampler& operator=(Resampler&&) = delete;
    Resampler& operator=(const Resampler&) = delete;
    ~Resampler() = default;

    bool initialize(FFMS_AudioSource* audioSource, const QAudioFormat& outAudioFormat, ErrorInfo& errorInfo)
    {
        m_audioProperties = FFMS_GetAudioProperties(audioSource);

        if (m_audioProperties->ChannelLayout != AudioChannelLayout_FFMS2
            || m_audioProperties->SampleFormat != AudioSampleFormat_FFMS2
            || m_audioProperties->SampleRate != outAudioFormat.sampleRate()) {
            ResampleContextPtr resampleContextPtr { swr_alloc() };
            av_opt_set_int(resampleContextPtr.get(), "in_channel_layout", m_audioProperties->ChannelLayout, 0);
            av_opt_set_int(resampleContextPtr.get(), "out_channel_layout", AudioChannelLayout_FFMS2, 0);
            av_opt_set_int(resampleContextPtr.get(), "in_sample_rate", m_audioProperties->SampleRate, 0);
            av_opt_set_int(resampleContextPtr.get(), "out_sample_rate", outAudioFormat.sampleRate(), 0);
            av_opt_set_sample_fmt(resampleContextPtr.get(), "in_sample_fmt", static_cast<AVSampleFormat>(m_audioProperties->SampleFormat), 0);
            av_opt_set_sample_fmt(resampleContextPtr.get(), "out_sample_fmt", static_cast<AVSampleFormat>(AudioSampleFormat_FFMS2), 0);
            if (swr_init(resampleContextPtr.get())) {
                errorInfo.set(FFMS_ERROR_RESAMPLING, FFMS_ERROR_UNSUPPORTED, "swr_init failed to initialize audio resampling");
                return false;
            }
            resampleContextPtr.swap(m_resampleContextPtr);

            // Allocate an output video frames worth of audio source buffer frames
            m_sourceAudioDataFrameCount = std::ceil(m_audioProperties->SampleRate * (duration<double, std::micro>(MediaManager::singletonInstance()->outputVideoFrameDuration()) / 1s));
            int sourceBufferSize = av_samples_get_buffer_size(NULL, m_audioProperties->Channels, m_sourceAudioDataFrameCount, static_cast<AVSampleFormat>(m_audioProperties->SampleFormat), 1);
            m_sourceAudioData = QByteArray(sourceBufferSize, Qt::Uninitialized);
        }
        return true;
    }

    int64_t read(FFMS_AudioSource* audioSource, void* buffer, int64_t startFrame, int64_t frameCount, ErrorInfo& errorInfo)
    {
        if (startFrame >= m_audioProperties->NumSamples)
            return 0;
        if (startFrame + frameCount >= m_audioProperties->NumSamples)
            frameCount = m_audioProperties->NumSamples - startFrame;
        if (FFMS_GetAudio(audioSource, buffer, startFrame, frameCount, &errorInfo) != 0) {
            return -1;
        }
        return frameCount;
    }

    void padSilence(QAudioBuffer& outAudioBuffer, int64_t startFrame)
    {
        qsizetype frameCount = outAudioBuffer.frameCount();
        if (startFrame < frameCount) {
            auto data = outAudioBuffer.data<float>();
            int channelCount = outAudioBuffer.format().channelCount();
            qsizetype sampleCount = channelCount * frameCount;
            for (int64_t i = startFrame * channelCount; i < sampleCount; i++) {
                data[i] = 0.0; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            }
        }
    }

    int resampleConvert(uint8_t** out, int outCount, const uint8_t** in, int inCount, ErrorInfo& errorInfo)
    {
        int count = swr_convert(m_resampleContextPtr.get(), out, outCount, in, inCount);
        if (count == -1)
            errorInfo.set(FFMS_ERROR_RESAMPLING, FFMS_ERROR_UNSUPPORTED, "swr_convert failed to resample, code "s + std::to_string(count));
        return count;
    }

    bool resampleInternal(FFMS_AudioSource* audioSource, QAudioBuffer& outAudioBuffer, ErrorInfo& errorInfo)
    {
        auto outData = outAudioBuffer.data<uint8_t>();
        int outFrameCount = static_cast<int>(outAudioBuffer.frameCount());

        // Flush any leftover input samples
        int leftoverFrameCount = resampleConvert(&outData, outFrameCount, NULL, 0, errorInfo);
        if (leftoverFrameCount == -1)
            return false;
        outFrameCount -= leftoverFrameCount;
        outData += leftoverFrameCount * outAudioBuffer.format().bytesPerFrame(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        while (outFrameCount > 0) {
            int sourceFrameCount = static_cast<int>(read(audioSource, m_sourceAudioData.data(), m_nextAudioFrameNum, m_sourceAudioDataFrameCount, errorInfo));
            if (sourceFrameCount == -1)
                return false;
            if (sourceFrameCount == 0) {
                padSilence(outAudioBuffer, outFrameCount);
                return true;
            }
            m_nextAudioFrameNum += sourceFrameCount;

            auto sourceData = m_sourceAudioData.constData();
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            int convertedFrameCount = resampleConvert(&outData, outFrameCount, reinterpret_cast<const uint8_t**>(&sourceData), sourceFrameCount, errorInfo);
            if (convertedFrameCount == -1)
                return false;
            outFrameCount -= convertedFrameCount;
            outData += convertedFrameCount * outAudioBuffer.format().bytesPerFrame(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        return true;
    }

    bool resample(FFMS_AudioSource* audioSource, const Interval& frameTime, QAudioBuffer& outAudioBuffer, ErrorInfo& errorInfo)
    {
        if (m_nextAudioFrameNum == -1) {
            // Audio start time in fractional seconds (since samplerate is per second)
            auto audioStartTime = duration<double>(frameTime.usStart() / frameTime.usDuration().count());
            // Start audio at the beginning of the current output frame time
            m_nextAudioFrameNum = static_cast<int64_t>(std::round((audioStartTime * m_audioProperties->SampleRate).count()));
        }

        if (m_resampleContextPtr) {
            return resampleInternal(audioSource, outAudioBuffer, errorInfo);
        } else {
            // No need to resample, read directly into output buffer
            // FFMS2/ffmpeg call "samples" what Qt calls "frames"
            int64_t frameCount = read(audioSource, outAudioBuffer.data<void>(), m_nextAudioFrameNum, outAudioBuffer.frameCount(), errorInfo);
            if (frameCount == -1) {
                return false;
            }
            padSilence(outAudioBuffer, frameCount);
            m_nextAudioFrameNum += outAudioBuffer.frameCount();
        }
        return true;
    };

private:
    typedef std::unique_ptr<SwrContext, ResampleContextDeleter> ResampleContextPtr;
    ResampleContextPtr m_resampleContextPtr;
    const FFMS_AudioProperties* m_audioProperties = nullptr;
    QByteArray m_sourceAudioData;
    int m_sourceAudioDataFrameCount = 0;
    int64_t m_nextAudioFrameNum = -1;
};

AudioTrack::AudioTrack(MediaClip* mediaClip)
    : Track(mediaClip)
{
}

AudioTrack::~AudioTrack() = default;

bool AudioTrack::initialize(FFMS_Index* index, int trackNum, const char* sourceFile, ErrorInfo& errorInfo)
{
    AudioSourcePtr audioSourcePtr { FFMS_CreateAudioSource(sourceFile, trackNum, index, FFMS_DELAY_FIRST_VIDEO_TRACK, &errorInfo) };
    if (audioSourcePtr) {
        auto manager = MediaManager::singletonInstance();
        m_outputAudioBuffer = manager->createOutputAudioBuffer();
        std::unique_ptr<Resampler> resamplerPtr { new Resampler() };
        if (!resamplerPtr->initialize(audioSourcePtr.get(), m_outputAudioBuffer.format(), errorInfo)) {
            return false;
        }
        m_resamplerPtr.swap(resamplerPtr);
        m_audioSourcePtr.swap(audioSourcePtr);
        updateActive();
        return true;
    }
    return false;
}

qint64 AudioTrack::duration() const
{
    if (!m_audioSourcePtr)
        return 0;
    const FFMS_AudioProperties* audioProps = FFMS_GetAudioProperties(m_audioSourcePtr.get());
    std::chrono::duration<float> endTime(audioProps->LastEndTime);
    return std::chrono::round<std::chrono::milliseconds>(endTime).count();
}

void AudioTrack::updateActive()
{
    setActive((bool)m_audioSourcePtr);
}

void AudioTrack::render(const Interval& frameTime, AudioRenderer* audioRenderer)
{
    ErrorInfo errorInfo;
    if (!m_resamplerPtr->resample(m_audioSourcePtr.get(), frameTime, m_outputAudioBuffer, errorInfo)) {
        qmlWarning(mediaClip()) << "Failed to decode audio:" << errorInfo;
        MediaManager::singletonInstance()->fatalError();
        return;
    }
    if (audioRenderer)
        audioRenderer->addAudioBuffer(m_outputAudioBuffer);
}

void AudioTrack::stop()
{
    m_audioSourcePtr.reset(nullptr);
    updateActive();
}