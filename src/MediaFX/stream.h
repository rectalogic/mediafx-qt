// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <chrono>
#include <memory>
#include <stdint.h>
extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/rational.h>
}
class AVCodecContext;
class AVFilter;
class AVFilterContext;
class AVFormatContext;
class AVFrame;
class Filter;
using namespace std::chrono;

struct FreeFrame {
    void operator()(AVFrame* frame) const;
};

class Stream : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    explicit Stream(const microseconds& startTime, const microseconds& frameDuration);
    Stream(Stream&&) = delete;
    Stream& operator=(Stream&&) = delete;
    ~Stream() override;

    int open(AVFormatContext* formatContext, AVMediaType mediaType, int relatedStreamIndex);
    bool isSinkFrameTimeValid(int64_t pts);
    int bufferSrcAddFrame(AVFrame* frame);
    virtual void processFrame(AVFrame* frame);

    int streamIndex() const { return m_streamIndex; }
    virtual const char* streamType() const = 0;
    bool isEOF() const { return m_streamEOF; }
    AVCodecContext* codecContext() const { return m_codecContext; }
    AVFilterContext* bufferSrcContext() const;
    AVFilterContext* bufferSinkContext() const;
    AVFrame* frame() const { return m_frame.get(); }
    AVFrame* filterFrame() const { return m_filterFrame.get(); }
    virtual void logAVFrame(void* context, int level, const AVFrame* frame) const = 0;

signals:
    void errorMessage(const QString& message);

protected:
    constexpr const microseconds& outputFrameDuration() const { return m_frameDuration; }
    virtual void createBuffers(const AVFilter** bufferSrc, const AVFilter** bufferSink) = 0;
    virtual QString createBufferSrcArgs(const AVRational& timeBase) = 0;
    virtual int configureBufferSink() = 0;
    virtual QString configureFilters() = 0;
    virtual void postConfigureBufferSink() = 0;

private:
    Q_DISABLE_COPY(Stream);

    microseconds m_startTime;
    microseconds m_frameDuration;
    int m_streamIndex = -1;
    bool m_streamEOF = false;
    bool m_bufferSrcEOF = false;
    AVCodecContext* m_codecContext = nullptr;
    std::unique_ptr<Filter> m_filter;
    std::unique_ptr<AVFrame, FreeFrame> m_frame;
    std::unique_ptr<AVFrame, FreeFrame> m_filterFrame;
};