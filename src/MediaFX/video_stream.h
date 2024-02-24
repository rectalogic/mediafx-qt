// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "stream.h"
#include "util.h"
#include <QString>
#include <QVideoFrame>
#include <chrono>
#include <utility>
extern "C" {
#include <libavutil/frame.h>
#include <libavutil/rational.h>
}
class AVFilter;

class VideoStream : public Stream {
public:
    explicit VideoStream(const AVRational& outputFrameRate, const microseconds& startTime)
        : Stream(startTime, frameRateToFrameDuration<microseconds>(outputFrameRate))
        , m_outputFrameRate(outputFrameRate)
    {
    }
    void processFrame(AVFrame* frame) override;

    QVideoFrame& outputVideoFrame() { return m_videoFrames.first; }
    const char* streamType() const override { return "video"; }
    void logAVFrame(void* context, int level, const AVFrame* frame) const override;

protected:
    void createBuffers(const AVFilter** bufferSrc, const AVFilter** bufferSink) override;
    QString createBufferSrcArgs(const AVRational& timeBase) override;
    int configureBufferSink() override;
    QString configureFilters() override;
    void postConfigureBufferSink() override { }

private:
    AVRational m_outputFrameRate;
    std::pair<QVideoFrame, QVideoFrame> m_videoFrames;
};