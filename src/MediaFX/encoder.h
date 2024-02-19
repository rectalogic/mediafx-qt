// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSize>
#include <QString>
#include <chrono>
#include <memory>
extern "C" {
#include <libavutil/rational.h>
}
class OutputStream;
class QAudioBuffer;
class QByteArray;
struct AVFormatContext;
using namespace std::chrono;

class Encoder {
public:
    Encoder(const QString& outputFile, const QSize& outputFrameSize, const AVRational& outputFrameRate, int outputSampleRate);
    Encoder(Encoder&&) = delete;
    Encoder(const Encoder&) = delete;
    Encoder& operator=(Encoder&&) = delete;
    Encoder& operator=(const Encoder&) = delete;
    ~Encoder();
    bool initialize();
    bool encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData);
    bool finish();
    constexpr const QSize& outputFrameSize() const noexcept { return m_outputFrameSize; };
    constexpr const AVRational& outputFrameRate() const noexcept { return m_outputFrameRate; };
    constexpr int outputSampleRate() const noexcept { return m_outputSampleRate; };

private:
    QSize m_outputFrameSize;
    AVRational m_outputFrameRate;
    int m_outputSampleRate;
    QString m_outputFile;
    AVFormatContext* m_formatContext = nullptr;
    std::unique_ptr<OutputStream> m_videoStream;
    std::unique_ptr<OutputStream> m_audioStream;
};