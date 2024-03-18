// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSize>
extern "C" {
#include <libavutil/rational.h>
}

class OutputFormat {
public:
    explicit constexpr OutputFormat(const QSize& frameSize, const AVRational& frameRate, int sampleRate)
        : m_frameSize(frameSize)
        , m_frameRate(frameRate)
        , m_sampleRate(sampleRate)
    {
    }
    OutputFormat(OutputFormat&&) = default;
    OutputFormat(const OutputFormat&) = default;
    OutputFormat& operator=(OutputFormat&&) = default;
    OutputFormat& operator=(const OutputFormat&) = default;
    ~OutputFormat() = default;

    constexpr const QSize& frameSize() const noexcept { return m_frameSize; };
    constexpr const AVRational& frameRate() const noexcept { return m_frameRate; };
    constexpr int sampleRate() const noexcept { return m_sampleRate; };

private:
    QSize m_frameSize;
    AVRational m_frameRate;
    int m_sampleRate;
};
