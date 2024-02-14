// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * mux raw audio (float) and video (RGBA) streams into NUT format
 * We bypass encoding and just stuff our raw data into AVPacket.data for
 * the muxer since it is already in the correct format.
 * http://git.ffmpeg.org/gitweb/nut.git
 * https://ffmpeg.org/nut.html
 */

#include "encoder.h"
#include "audio.h"
#include <QAudioBuffer>
#include <QDebug>
#include <QObject>
#include <QtLogging>
#include <array>
#include <stdint.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/attributes.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <libavutil/version.h>
}

av_always_inline QString av_err2qstring(int errnum)
{
    std::array<char, AV_ERROR_MAX_STRING_SIZE> str; // NOLINT(cppcoreguidelines-pro-type-member-init)
    return av_make_error_string(str.data(), AV_ERROR_MAX_STRING_SIZE, errnum);
}

class OutputStream {
public:
    explicit OutputStream() = default;
    OutputStream(OutputStream&&) = delete;
    OutputStream(const OutputStream&) = delete;
    OutputStream& operator=(OutputStream&&) = delete;
    OutputStream& operator=(const OutputStream&) = delete;
    ~OutputStream()
    {
        if (m_codecCtx)
            avcodec_free_context(&m_codecCtx);
        if (m_pkt)
            av_packet_free(&m_pkt);
    };

    bool initialize(AVFormatContext* formatCtx, enum AVCodecID codecID)
    {
        m_codec = avcodec_find_encoder(codecID);
        if (!m_codec) {
            qCritical() << "Could not find encoder for" << avcodec_get_name(codecID);
            return false;
        }

        m_pkt = av_packet_alloc();
        if (!m_pkt) {
            qCritical() << "Could not allocate AVPacket, av_packet_alloc";
            return false;
        }

        m_stream = avformat_new_stream(formatCtx, nullptr);
        if (!m_stream) {
            qCritical() << "Could not allocate stream, avformat_new_stream";
            return false;
        }
        m_stream->id = static_cast<int>(formatCtx->nb_streams - 1);
        m_codecCtx = avcodec_alloc_context3(m_codec);
        if (!m_codecCtx) {
            qCritical() << "Could not allocate an encoding context, avcodec_alloc_context3";
            return false;
        }
        return true;
    };

    bool open()
    {
        int ret = avcodec_open2(m_codecCtx, m_codec, nullptr);
        if (ret < 0) {
            qCritical() << "Could not open cocdec, avcodec_open2: " << av_err2qstring(ret);
            return false;
        }
        ret = avcodec_parameters_from_context(m_stream->codecpar, m_codecCtx);
        if (ret < 0) {
            qCritical() << "Could not copy codec parameters, avcodec_parameters_from_context: " << av_err2qstring(ret);
            return false;
        }
        return true;
    };

    bool writePacket(AVFormatContext* formatCtx, int64_t ptsIncrement)
    {
        m_pkt->pts = m_nextPTS;
        m_pkt->dts = m_nextPTS;
        av_packet_rescale_ts(m_pkt, m_codecCtx->time_base, m_stream->time_base);
        m_pkt->stream_index = m_stream->index;
        int ret = av_write_frame(formatCtx, m_pkt);
        if (ret < 0) {
            qCritical() << "Could not write frame, av_write_frame: " << av_err2qstring(ret);
            return false;
        }
        m_nextPTS += ptsIncrement;
        return true;
    };

    const AVCodec* codec() const { return m_codec; };
    AVCodecContext* codecContext() const { return m_codecCtx; };
    AVStream* stream() const { return m_stream; };
    AVPacket* packet() const { return m_pkt; };

private:
    AVStream* m_stream = nullptr;
    const AVCodec* m_codec = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    AVPacket* m_pkt = nullptr;
    int64_t m_nextPTS = 0;
};

Encoder::Encoder(const QString& outputFile, const FrameSize& outputFrameSize, const FrameRate& outputFrameRate, int outputSampleRate, QObject* parent)
    : QObject(parent)
    , m_outputFile(outputFile)
    , m_outputFrameSize(outputFrameSize)
    , m_outputFrameRate(outputFrameRate)
    , m_outputSampleRate(outputSampleRate) {};

Encoder::~Encoder()
{
    m_audioStream.release();
    m_videoStream.release();
    if (m_formatCtx)
        avformat_free_context(m_formatCtx);
}

bool Encoder::initialize()
{
    int ret = 0;
    // Select nut format
    if ((ret = avformat_alloc_output_context2(&m_formatCtx, nullptr, "nut", m_outputFile.toUtf8().constData())) < 0) {
        qCritical() << "Could not allocate an output context, error:" << av_err2qstring(ret);
        return false;
    }

    // Video stream, AV_CODEC_ID_RAWVIDEO/AV_PIX_FMT_RGBA
    std::unique_ptr<OutputStream> video(new OutputStream());
    if (!video->initialize(m_formatCtx, AV_CODEC_ID_RAWVIDEO))
        return false;
    AVCodecContext* videoCodecCtx = video->codecContext();
    videoCodecCtx->pix_fmt = AV_PIX_FMT_RGBA;
    videoCodecCtx->width = m_outputFrameSize.width();
    videoCodecCtx->height = m_outputFrameSize.height();
    AVRational timeBase = { m_outputFrameRate.den(), m_outputFrameRate.num() };
    int64_t gcd = av_gcd(FFABS(timeBase.num), FFABS(timeBase.den));
    if (gcd) {
        timeBase.num = FFABS(timeBase.num) / gcd;
        timeBase.den = FFABS(timeBase.den) / gcd;
    }
    video->stream()->time_base = videoCodecCtx->time_base = timeBase;
    if (!video->open())
        return false;

    // Audio stream, AV_CODEC_ID_PCM_F32(BE|LE)/AV_SAMPLE_FMT_FLT
    std::unique_ptr<OutputStream> audio(new OutputStream());
    if (!audio->initialize(m_formatCtx, AudioCodec_FFMPEG))
        return false;
    AVCodecContext* audioCodecCtx = audio->codecContext();
    audioCodecCtx->sample_fmt = AudioSampleFormat_FFMPEG;
    audioCodecCtx->sample_rate = m_outputSampleRate;
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(57, 28, 100)
    audioCodecCtx->channel_layout = AudioChannelLayout_FFMPEG;
    audioCodecCtx->channels = av_get_channel_layout_nb_channels(audioCodecCtx->channel_layout);
#else
    if ((ret = av_channel_layout_copy(&audioCodecCtx->ch_layout, &AudioChannelLayout_FFMPEG)) < 0) {
        qCritical() << "Could not copy channel layout, error:" << av_err2qstring(ret);
        return false;
    }
#endif
    audio->stream()->time_base = audioCodecCtx->time_base = (AVRational) { 1, audioCodecCtx->sample_rate };
    if (!audio->open())
        return false;

    m_audioStream.swap(audio);
    m_videoStream.swap(video);

    if (!(m_formatCtx->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&m_formatCtx->pb, m_outputFile.toUtf8().constData(), AVIO_FLAG_WRITE)) < 0) {
            qCritical() << "Could not open output file" << m_outputFile << ", avio_open:" << av_err2qstring(ret);
            return false;
        }
    }
    AVDictionary* opt = nullptr;
    if ((ret = av_dict_set(&opt, "fflags", "bitexact", 0)) < 0) {
        qCritical() << "Could not set options, av_dict_set:" << av_err2qstring(ret);
        return false;
    }
    ret = avformat_write_header(m_formatCtx, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        qCritical() << "Could not open output file, avio_open:" << av_err2qstring(ret);
        return false;
    }

    return true;
}

bool Encoder::encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData)
{
    AVPacket* videoPacket = m_videoStream->packet();
    videoPacket->flags |= AV_PKT_FLAG_KEY;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast, cppcoreguidelines-pro-type-reinterpret-cast)
    videoPacket->data = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(videoData.constData()));
    videoPacket->size = static_cast<int>(videoData.size());
    if (!m_videoStream->writePacket(m_formatCtx, 1))
        return false;

    AVPacket* audioPacket = m_audioStream->packet();
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    audioPacket->data = const_cast<uint8_t*>(audioBuffer.constData<uint8_t>());
    audioPacket->size = static_cast<int>(audioBuffer.byteCount());
    if (!m_audioStream->writePacket(m_formatCtx, audioBuffer.frameCount()))
        return false;

    return true;
}

bool Encoder::finish()
{
    int ret = 0;
    if ((ret = av_write_trailer(m_formatCtx)) < 0) {
        qCritical() << "Could not write trailer, av_write_trailer:" << av_err2qstring(ret);
        return false;
    }
    if (!(m_formatCtx->flags & AVFMT_NOFILE)) {
        if ((ret = avio_closep(&m_formatCtx->pb)) < 0) {
            qCritical() << "Could not close file, avio_closep:" << av_err2qstring(ret);
            return false;
        }
    }
    return true;
}