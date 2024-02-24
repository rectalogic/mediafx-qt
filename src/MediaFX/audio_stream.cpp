// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_stream.h"
#include "formats.h"
#include "util.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QObject>
#include <QString>
#include <QtCore>
#include <array>
#include <cstring>
#include <stdint.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <libavutil/rational.h>
#include <libavutil/samplefmt.h>
#include <libavutil/version.h>
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(59, 37, 100)
#include <libavutil/bprint.h>
#endif
}
using namespace Qt::Literals::StringLiterals;

// NOLINTBEGIN(bugprone-assignment-in-if-condition)

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void AudioStream::createBuffers(const AVFilter** bufferSrc, const AVFilter** bufferSink)
{
    *bufferSrc = avfilter_get_by_name("abuffer");
    *bufferSink = avfilter_get_by_name("abuffersink");
}

QString AudioStream::createBufferSrcArgs(const AVRational& timeBase)
{
    auto audioCodecContext = codecContext();
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 37, 100)
    if (!audioCodecContext->channel_layout)
        audioCodecContext->channel_layout = av_get_default_channel_layout(audioCodecContext->channels);
#else
    if (audioCodecContext->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
        av_channel_layout_default(&audioCodecContext->ch_layout, audioCodecContext->ch_layout.nb_channels);
#endif
    QString args = u"time_base=%1/%2:sample_rate=%3:sample_fmt=%4"_s.arg(
        QString::number(timeBase.num), QString::number(timeBase.den),
        QString::number(audioCodecContext->sample_rate),
        av_get_sample_fmt_name(audioCodecContext->sample_fmt));
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 37, 100)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    args.append(u":channel_layout=0x%1"_s.arg(audioCodecContext->channel_layout, 0, 16));
#else
    AVBPrint bprint;
    av_bprint_init(&bprint, 0, AV_BPRINT_SIZE_UNLIMITED);
    av_channel_layout_describe_bprint(&audioCodecContext->ch_layout, &bprint);
    args.append(u":channel_layout=%1"_s.arg(bprint.str));
    av_bprint_finalize(&bprint, NULL);
#endif

    return args;
}

int AudioStream::configureBufferSink()
{
    int ret = 0;
    static const std::array<enum AVSampleFormat, 2> sampleFormats { AudioSampleFormat_FFMPEG, AV_SAMPLE_FMT_NONE };
    std::array<int, 2> sampleRates { m_outputAudioFormat.sampleRate(), -1 };

    if ((ret = av_opt_set_int_list(bufferSinkContext(), "sample_fmts", sampleFormats.data(), AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output audio sample format: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 37, 100)
    static const std::array<int64_t, 2> channelLayouts { AudioChannelLayout_FFMPEG, -1 };
    if ((ret = av_opt_set_int_list(bufferSinkContext(), "channel_layouts", channelLayouts.data(), -1, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output audio channel layout: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
#else
    if ((ret = av_opt_set(bufferSinkContext(), "ch_layouts", AudioChannelLayoutName_FFMPEG, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output audio channel layout: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
#endif

    if ((ret = av_opt_set_int_list(bufferSinkContext(), "sample_rates", sampleRates.data(), -1, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output audio sample rate: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }

    m_outputAudioBuffer = QAudioBuffer(outputAudioFrameCount(), m_outputAudioFormat);

    return ret;
}

QString AudioStream::configureFilters()
{
    QString filters = u"aresample=%1:first_pts=0:out_sample_fmt=%2"_s.arg(
        QString::number(m_outputAudioBuffer.format().sampleRate()),
        av_get_sample_fmt_name(AudioSampleFormat_FFMPEG));
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 37, 100)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    filters.append(u":out_channel_layout=0x%1"_s.arg(AudioChannelLayout_FFMPEG, 0, 16));
#else
    filters.append(u":out_chlayout=%1"_s.arg(AudioChannelLayoutName_FFMPEG));
#endif
    return filters;
}

void AudioStream::postConfigureBufferSink()
{
    // Ensure we always get all samples we need for a frame
    av_buffersink_set_frame_size(bufferSinkContext(), outputAudioFrameCount());
}

void AudioStream::processFrame(AVFrame* frame)
{
    Stream::processFrame(frame);
    if (!frame) {
        m_outputAudioBuffer = QAudioBuffer();
        return;
    }
    // Despite docs, av_buffersink_set_frame_size does not zero pad the last frame. It can be short.
    // https://trac.ffmpeg.org/ticket/10888
    // Don't use frame->linesize[0], it can be larger than the data.
    // https://ffmpeg.org/doxygen/trunk/structAVFrame.html#aa52bfc6605f6a3059a0c3226cc0f6567
    Q_ASSERT(m_outputAudioBuffer.frameCount() >= frame->nb_samples);
    int frameSize = frame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format)) *
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(59, 37, 100)
        frame->channels;
#else
        frame->ch_layout.nb_channels;
#endif
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::memcpy(m_outputAudioBuffer.data<uint8_t>(), frame->data[0], frameSize);
    if (frameSize < m_outputAudioBuffer.byteCount())
        std::memset(&(m_outputAudioBuffer.data<uint8_t>()[frameSize]), 0, m_outputAudioBuffer.byteCount() - frameSize);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

void AudioStream::logAVFrame(void* context, int level, const AVFrame* frame) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    av_log(context, level, "nb_samples: %d format: %s sample_rate: %d\n",
        frame->nb_samples, av_get_sample_fmt_name(static_cast<AVSampleFormat>(frame->format)), frame->sample_rate);
}

// NOLINTEND(bugprone-assignment-in-if-condition)