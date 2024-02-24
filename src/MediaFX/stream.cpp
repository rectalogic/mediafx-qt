// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stream.h"
#include "util.h"
#include <QString>
#include <chrono>
#include <compare>
#include <errno.h>
#include <ratio>
#include <stddef.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavformat/version.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavutil/version.h>
}
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace Qt::Literals::StringLiterals;

// NOLINTBEGIN(bugprone-assignment-in-if-condition)

void FreeFrame::operator()(AVFrame* frame) const
{
    av_frame_free(&frame);
}

class Filter {
public:
    Filter(AVFilterGraph* filterGraph)
        : m_filterGraph(filterGraph)
    {
    }
    Filter(Filter&&) = delete;
    Filter(const Filter&) = delete;
    Filter& operator=(Filter&&) = delete;
    Filter& operator=(const Filter&) = delete;
    ~Filter()
    {
        avfilter_graph_free(&m_filterGraph);
    }

    AVFilterGraph* filterGraph() const { return m_filterGraph; }
    void setBufferSrcContext(AVFilterContext* context) { m_bufferSrcContext = context; }
    AVFilterContext* bufferSrcContext() const { return m_bufferSrcContext; }
    void setBufferSinkContext(AVFilterContext* context) { m_bufferSinkContext = context; }
    AVFilterContext* bufferSinkContext() const { return m_bufferSinkContext; }

private:
    AVFilterGraph* m_filterGraph;
    AVFilterContext* m_bufferSrcContext = nullptr;
    AVFilterContext* m_bufferSinkContext = nullptr;
};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Stream::Stream(const microseconds& startTime, const microseconds& frameDuration)
    : QObject()
    , m_startTime(startTime)
    , m_frameDuration(frameDuration)
{
}

Stream::~Stream()
{
    m_filter.reset();
    avcodec_free_context(&m_codecContext);
}

int Stream::open(AVFormatContext* formatContext, AVMediaType mediaType, int relatedStreamIndex)
{
    int ret = 0;
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(59, 0, 100)
#define CONST const
#else
#define CONST
#endif
    CONST AVCodec* codec = nullptr;
    m_streamIndex = av_find_best_stream(formatContext, mediaType, -1, relatedStreamIndex, &codec, 0);
    if (m_streamIndex < 0) {
        if (m_streamIndex != AVERROR_STREAM_NOT_FOUND)
            emit errorMessage(u"Failed to find %1 stream - av_find_best_stream failed: %2"_s.arg(streamType(), av_err2qstring(m_streamIndex)));
        return m_streamIndex;
    }

    m_frame = std::unique_ptr<AVFrame, FreeFrame>(av_frame_alloc());
    m_filterFrame = std::unique_ptr<AVFrame, FreeFrame>(av_frame_alloc());
    if (!m_frame || !m_filterFrame) {
        emit errorMessage(u"Failed to allocate frames. av_frame_alloc"_s);
        return AVERROR(ENOMEM);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    AVStream* avstream = formatContext->streams[m_streamIndex];

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        emit errorMessage(u"%1 stream avcodec_alloc_context3 failed"_s.arg(streamType()));
        return AVERROR(ENOMEM);
    }
    if ((ret = avcodec_parameters_to_context(m_codecContext, avstream->codecpar)) < 0) {
        emit errorMessage(u"%1 stream avcodec_parameters_to_context failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }
    m_codecContext->pkt_timebase = avstream->time_base;
    if ((ret = avcodec_open2(m_codecContext, codec, NULL)) < 0) {
        emit errorMessage(u"%1 stream avcodec_open2 failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }

    const AVFilter* bufferSrc = nullptr;
    const AVFilter* bufferSink = nullptr;

    createBuffers(&bufferSrc, &bufferSink);

    m_filter = std::make_unique<Filter>(avfilter_graph_alloc());
    if (!m_filter->filterGraph()) {
        emit errorMessage(u"%1 stream avfilter_graph_alloc failed"_s.arg(streamType()));
        return AVERROR(ENOMEM);
    }

    QString bufferSrcArgs = createBufferSrcArgs(avstream->time_base);
    AVFilterContext* bufferSrcContext = nullptr;
    if ((ret = avfilter_graph_create_filter(&bufferSrcContext, bufferSrc, "in", qUtf8Printable(bufferSrcArgs), NULL, m_filter->filterGraph())) < 0) {
        emit errorMessage(u"%1 stream buffersrc avfilter_graph_create_filter failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }
    m_filter->setBufferSrcContext(bufferSrcContext);

    AVFilterContext* bufferSinkContext = nullptr;
    if ((ret = avfilter_graph_create_filter(&bufferSinkContext, bufferSink, "out", NULL, NULL, m_filter->filterGraph())) < 0) {
        emit errorMessage(u"%1 stream buffersink avfilter_graph_create_filter failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }
    m_filter->setBufferSinkContext(bufferSinkContext);

    if ((ret = configureBufferSink()) < 0)
        return ret;

    AVFilterInOut* outputs { avfilter_inout_alloc() };
    AVFilterInOut* inputs { avfilter_inout_alloc() };
    if (!outputs || !inputs) {
        avfilter_inout_free(&outputs);
        avfilter_inout_free(&inputs);
        emit errorMessage(u"%1 stream avfilter_inout_alloc failed"_s.arg(streamType()));
        return AVERROR(ENOMEM);
    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = m_filter->bufferSrcContext();
    outputs->pad_idx = 0;
    outputs->next = nullptr;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = m_filter->bufferSinkContext();
    inputs->pad_idx = 0;
    inputs->next = nullptr;

    QString filters = configureFilters();
    ret = avfilter_graph_parse_ptr(m_filter->filterGraph(), qUtf8Printable(filters), &inputs, &outputs, NULL);
    avfilter_inout_free(&outputs);
    avfilter_inout_free(&inputs);
    if (ret < 0) {
        emit errorMessage(u"%1 stream avfilter_graph_parse_ptr failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }

    if ((ret = avfilter_graph_config(m_filter->filterGraph(), NULL)) < 0) {
        emit errorMessage(u"%1 stream avfilter_graph_config failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }

    postConfigureBufferSink();
    return 0;
}

AVFilterContext* Stream::bufferSrcContext() const
{
    return m_filter->bufferSrcContext();
}

AVFilterContext* Stream::bufferSinkContext() const
{
    return m_filter->bufferSinkContext();
}

bool Stream::isSinkFrameTimeValid(int64_t pts)
{
    if (m_startTime <= 0us)
        return true;
    // NOLINTNEXTLINE(*-narrowing-conversions)
    duration<double> frameStartTime(pts * av_q2d(av_buffersink_get_time_base(bufferSinkContext())));
    if (frameStartTime >= m_startTime && m_startTime < (frameStartTime + m_frameDuration)) {
        // Flag so we stop checking
        m_startTime = -1us;
        return true;
    }
    return false;
}

int Stream::bufferSrcAddFrame(AVFrame* frame)
{
    int ret = 0;
    // Return if we already sent EOF
    if (m_bufferSrcEOF)
        return 0;
    if ((ret = av_buffersrc_add_frame_flags(bufferSrcContext(), frame, frame ? AV_BUFFERSRC_FLAG_KEEP_REF : AV_BUFFERSRC_FLAG_PUSH)) < 0) {
        emit errorMessage(u"%1 stream av_buffersrc_add_frame_flags failed: %2"_s.arg(streamType(), av_err2qstring(ret)));
        return ret;
    }
    if (!frame)
        m_bufferSrcEOF = true;
    return ret;
}

void Stream::processFrame(AVFrame* frame)
{
    if (!frame)
        m_streamEOF = true;
}

// NOLINTEND(bugprone-assignment-in-if-condition)