// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "stream.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QString>
#include <chrono>
extern "C" {
#include <libavutil/frame.h>
#include <libavutil/rational.h>
}
class AVFilter;

class AudioStream : public Stream {
public:
    explicit AudioStream(const QAudioFormat& outputAudioFormat, const microseconds& frameDuration, const microseconds& startTime)
        : Stream(startTime, frameDuration)
        , m_outputAudioFormat(outputAudioFormat)
    {
    }
    void processFrame(AVFrame* frame) override;

    QAudioBuffer& outputAudioBuffer() { return m_outputAudioBuffer; }
    const char* streamType() const override { return "audio"; }
    void logAVFrame(void* context, int level, const AVFrame* frame) const override;

protected:
    void createBuffers(const AVFilter** bufferSrc, const AVFilter** bufferSink) override;
    QString createBufferSrcArgs(const AVRational& timeBase) override;
    int configureBufferSink() override;
    QString configureFilters() override;
    void postConfigureBufferSink() override;

private:
    int outputAudioFrameCount() const
    {
        return m_outputAudioFormat.framesForDuration(outputFrameDuration().count());
    }

    QAudioFormat m_outputAudioFormat;
    QAudioBuffer m_outputAudioBuffer;
};