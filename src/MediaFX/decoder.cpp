// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "decoder.h"
#include "audio_stream.h"
#include "stream.h"
#include "util.h"
#include "video_stream.h"
#include <QChar>
#include <QString>
#include <chrono>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
}
using namespace std::chrono_literals;
using namespace Qt::Literals::StringLiterals;

// NOLINTBEGIN(bugprone-assignment-in-if-condition)

struct UnrefPacket {
    void operator()(AVPacket* packet) const
    {
        av_packet_unref(packet);
    }
};

void FreePacket::operator()(AVPacket* packet) const
{
    av_packet_free(&packet);
}

struct UnrefFrame {
    void operator()(AVFrame* frame) const
    {
        av_frame_unref(frame);
    }
};

void CloseFormatContext::operator()(AVFormatContext* formatContext) const
{
    avformat_close_input(&formatContext);
}

void logAVFrameInternal(const Stream* stream, void* context, const AVRational& timeBase, int level, const AVFrame* frame)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    av_log(context, level, "mediafx %s frame pts: %" PRId64 " tb: %d/%d ", stream->streamType(), frame->pts, timeBase.num, timeBase.den);
    stream->logAVFrame(context, level, frame);
}

void logAVFrame(const Stream* stream, const AVCodecContext* context, int level, const AVFrame* frame)
{
    if (level > av_log_get_level())
        return;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast,cppcoreguidelines-pro-type-reinterpret-cast)
    logAVFrameInternal(stream, const_cast<void*>(reinterpret_cast<const void*>(context)), context->time_base, level, frame);
}

void logAVFrame(const Stream* stream, const AVFilterContext* context, int level, const AVFrame* frame)
{
    if (level > av_log_get_level())
        return;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast,cppcoreguidelines-pro-type-reinterpret-cast)
    logAVFrameInternal(stream, const_cast<void*>(reinterpret_cast<const void*>(context)), av_buffersink_get_time_base(context), level, frame);
}

Decoder::Decoder(QObject* parent)
    : QObject(parent)
{
}

Decoder::~Decoder()
{
    stop();
}

void Decoder::stop()
{
    m_audioStream.reset();
    m_videoStream.reset();
    m_formatContext.reset();
    m_packet.reset();
}

int Decoder::open(const QString& sourceFile, const AVRational& outputFrameRate, const QAudioFormat& outputAudioFormat, const microseconds& startTime)
{
    int ret = 0;

    AVFormatContext* ctx = nullptr;
    if ((ret = avformat_open_input(&ctx, qUtf8Printable(sourceFile), NULL, NULL)) < 0) {
        emit errorMessage(u"Failed to open source file. avformat_open_input: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
    std::unique_ptr<AVFormatContext, CloseFormatContext> formatCtx(ctx);

    if ((ret = avformat_find_stream_info(formatCtx.get(), NULL)) < 0) {
        emit errorMessage(u"Failed to find stream info. avformat_find_stream_info: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }

    std::unique_ptr<VideoStream> videoStream(new VideoStream(outputFrameRate, startTime));
    connect(videoStream.get(), &VideoStream::errorMessage, this, &Decoder::errorMessage);
    if ((ret = videoStream->open(formatCtx.get(), AVMEDIA_TYPE_VIDEO, -1)) < 0) {
        videoStream.reset();
        if (ret != AVERROR_STREAM_NOT_FOUND && ret != AVERROR_DECODER_NOT_FOUND)
            return ret;
    }

    std::unique_ptr<AudioStream> audioStream(new AudioStream(outputAudioFormat, frameRateToFrameDuration<microseconds>(outputFrameRate), startTime));
    connect(audioStream.get(), &AudioStream::errorMessage, this, &Decoder::errorMessage);
    if ((ret = audioStream->open(formatCtx.get(), AVMEDIA_TYPE_AUDIO, videoStream ? videoStream->streamIndex() : -1)) < 0) {
        audioStream.reset();
        if (ret != AVERROR_STREAM_NOT_FOUND && ret != AVERROR_DECODER_NOT_FOUND)
            return ret;
    }

    if (!audioStream && !videoStream) {
        emit errorMessage(u"Failed to find audio or video stream"_s);
        return AVERROR_STREAM_NOT_FOUND;
    }

    // Discard other streams
    for (int streamIndex = 0; streamIndex < formatCtx->nb_streams; streamIndex++) {
        if (!((audioStream && audioStream->streamIndex() == streamIndex)
                || (videoStream && videoStream->streamIndex() == streamIndex))) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            formatCtx->streams[streamIndex]->discard = AVDISCARD_ALL;
        }
    }

    if (av_log_get_level() >= AV_LOG_INFO) {
        av_dump_format(formatCtx.get(), 0, qUtf8Printable(sourceFile), 0);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        av_log(static_cast<void*>(formatCtx.get()), AV_LOG_INFO, "Using streams - audio: #0:%d video: #0:%d\n",
            audioStream ? audioStream->streamIndex() : -1, videoStream ? videoStream->streamIndex() : -1);
    }

    std::unique_ptr<AVPacket, FreePacket> packet(av_packet_alloc());
    if (!packet) {
        emit errorMessage(u"Failed to allocate packet. av_packet_alloc"_s);
        return AVERROR(ENOMEM);
    }

    m_formatContext.swap(formatCtx);
    m_audioStream.swap(audioStream);
    m_videoStream.swap(videoStream);
    m_packet.swap(packet);
    return 0;
}

const microseconds Decoder::duration() const
{
    if (!m_formatContext)
        return -1us;
    microseconds frameDuration = 0us;
    if (m_videoStream) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        AVRational frameRate = av_guess_frame_rate(m_formatContext.get(), m_formatContext->streams[m_videoStream->streamIndex()], nullptr);
        if (frameRate.num)
            frameDuration = frameRateToFrameDuration<microseconds>(frameRate);
    }
    // AVFormatContext->duration is pts of last frame, so we add a frame duration to get the end time
    return std::chrono::duration<int64_t, std::ratio<1, AV_TIME_BASE>>(m_formatContext->duration) + frameDuration;
}

bool Decoder::pullFrameFromSink(Stream* stream, bool& gotFrame)
{
    int ret = 0;
    if (stream && !gotFrame) {
        std::unique_ptr<AVFrame, UnrefFrame> filterFrameRef;
        ret = av_buffersink_get_frame(stream->bufferSinkContext(), stream->filterFrame());
        if (ret == AVERROR_EOF) {
            // Signal EOF
            stream->processFrame(nullptr);
            gotFrame = true;
            return true;
        } else if (ret == AVERROR(EAGAIN)) {
            return true;
        } else if (ret < 0) {
            emit errorMessage(u"%1 stream av_buffersink_get_frame failed: %2"_s.arg(stream->streamType(), av_err2qstring(ret)));
            return false;
        }
        filterFrameRef.reset(stream->filterFrame());
        if (stream->isSinkFrameTimeValid(filterFrameRef->pts)) {
            logAVFrame(stream, stream->bufferSinkContext(), AV_LOG_DEBUG, filterFrameRef.get());
            stream->processFrame(filterFrameRef.get());
            gotFrame = true;
        }
    }
    return true;
}

bool Decoder::sendPacketToDecoder(Stream* stream, AVPacket* packet)
{
    int ret = 0;
    // Some formats return zero size packets before EOF, ignore them.
    // https://trac.ffmpeg.org/ticket/10849
    if (packet && packet->size == 0)
        return true;
    if (stream && (ret = avcodec_send_packet(stream->codecContext(), packet)) < 0) {
        emit errorMessage(u"%1 stream avcodec_send_packet failed: %2"_s.arg(stream->streamType(), av_err2qstring(ret)));
        return false;
    }
    return true;
}

bool Decoder::filter(Stream* stream, bool& gotFrame)
{
    if (!stream)
        return true;

    while (true) {
        std::unique_ptr<AVFrame, UnrefFrame> frameRef;
        int ret = avcodec_receive_frame(stream->codecContext(), stream->frame());
        if (ret == AVERROR(EAGAIN)) {
            break;
        } else if (ret == AVERROR_EOF) {
            // Signal EOF, null frame on EOF will close the buffersrc and initiate draining
            if (stream->bufferSrcAddFrame(nullptr) < 0)
                return false;
            break;
        } else if (ret < 0) {
            emit errorMessage(u"%1 stream avcodec_receive_frame failed: %2"_s.arg(stream->streamType(), av_err2qstring(ret)));
            return false;
        }

        frameRef.reset(stream->frame());
        frameRef->pts = frameRef->best_effort_timestamp;
        logAVFrame(stream, stream->codecContext(), AV_LOG_TRACE, frameRef.get());

        // If frame properties (dimensions, samplerate etc.) have changed, we need to reconfigure buffersrc and filtergraph.
        // (see ffmpeg need_reinit). But this would lose buffered data unless we create a new filtergraph and use
        // a queue of filtergraphs (write to the new tail buffersrc, read from the head buffersink)
        // Audio would need special handling since we rely on a specific number of samples per frame.

        // Push frame into filtergraph.
        if (stream->bufferSrcAddFrame(frameRef.get()) < 0)
            return false;
    }

    // Pull filtered frames from the filtergraph
    if (!pullFrameFromSink(stream, gotFrame))
        return false;

    return true;
}

bool Decoder::decode()
{
    Q_ASSERT(m_formatContext);
    int ret = 0;
    bool gotAudioFrame = !m_audioStream || m_audioStream->isEOF();
    bool gotVideoFrame = !m_videoStream || m_videoStream->isEOF();

    // Check for any buffered frames from last time

    if (!pullFrameFromSink(m_videoStream.get(), gotVideoFrame))
        return false;
    if (!pullFrameFromSink(m_audioStream.get(), gotAudioFrame))
        return false;

    while (!gotAudioFrame || !gotVideoFrame) {
        std::unique_ptr<AVPacket, UnrefPacket> packetRef;
        if (!m_formatEOF) {
            ret = av_read_frame(m_formatContext.get(), m_packet.get());
            if (ret >= 0) {
                AVPacket* pkt = m_packet.get();
                AVFormatContext* ctx = m_formatContext.get();
                packetRef.reset(pkt);
                av_log(static_cast<void*>(ctx), AV_LOG_TRACE, "mediafx "); // NOLINT(cppcoreguidelines-pro-type-vararg)
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                av_pkt_dump_log2(static_cast<void*>(ctx), AV_LOG_TRACE, pkt, 0, ctx->streams[pkt->stream_index]);
            } else if (ret == AVERROR_EOF) {
                // On EOF, we send a NULL packet to avcodec_send_packet to enter draining mode
                m_formatEOF = true;
                if (!sendPacketToDecoder(m_videoStream.get(), nullptr))
                    return false;
                if (!sendPacketToDecoder(m_audioStream.get(), nullptr))
                    return false;
            } else if (ret < 0) {
                emit errorMessage(u"Error demuxing, av_read_frame failed: %1"_s.arg(av_err2qstring(ret)));
                return false;
            }
        }

        if (packetRef) {
            Stream* stream = nullptr;
            bool* gotFrame = nullptr;
            if (m_videoStream && packetRef->stream_index == m_videoStream->streamIndex()) {
                stream = m_videoStream.get();
                gotFrame = &gotVideoFrame;
            } else if (m_audioStream && packetRef->stream_index == m_audioStream->streamIndex()) {
                stream = m_audioStream.get();
                gotFrame = &gotAudioFrame;
            } else {
                // Some unrecognized stream type
                continue;
            }

            if (!sendPacketToDecoder(stream, packetRef.get()))
                return false;
            if (!filter(stream, *gotFrame))
                return false;
        } else {
            // If no packet, then we are draining. Pull from both streams.

            if (!filter(m_videoStream.get(), gotVideoFrame))
                return false;
            if (!filter(m_audioStream.get(), gotAudioFrame))
                return false;
        }
    }
    return true;
}

bool Decoder::isAudioEOF() const
{
    return m_audioStream ? m_audioStream->isEOF() : true;
}

bool Decoder::isVideoEOF() const
{
    return m_videoStream ? m_videoStream->isEOF() : true;
}

QVideoFrame Decoder::outputVideoFrame() const
{
    return m_videoStream ? m_videoStream->outputVideoFrame() : QVideoFrame();
}

QAudioBuffer Decoder::outputAudioBuffer() const
{
    return m_audioStream ? m_audioStream->outputAudioBuffer() : QAudioBuffer();
}

// NOLINTEND(bugprone-assignment-in-if-condition)