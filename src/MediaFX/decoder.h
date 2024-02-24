// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "util.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QObject>
#include <QString>
#include <QVideoFrame>
#include <chrono>
#include <memory>
extern "C" {
#include <libavcodec/packet.h>
#include <libavutil/frame.h>
#include <libavutil/rational.h>
}
Q_MOC_INCLUDE("audio_stream.h")
Q_MOC_INCLUDE("video_stream.h")
class AudioStream;
class Stream;
class VideoStream;
struct AVFormatContext;
using namespace std::chrono;

struct CloseFormatContext {
    void operator()(AVFormatContext* formatContext) const;
};
struct FreePacket {
    void operator()(AVPacket* packet) const;
};

class Decoder : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    Decoder(QObject* parent = nullptr);
    Decoder(Decoder&&) = delete;
    Decoder& operator=(Decoder&&) = delete;
    ~Decoder() override;

    int open(const QString& sourceFile, const AVRational& outputFrameRate, const QAudioFormat& outputAudioFormat, const microseconds& startTime = 0us);
    bool decode();
    void stop();

    const microseconds duration() const;

    bool hasAudio() const { return m_audioStream != nullptr; }
    bool isAudioEOF() const;
    bool hasVideo() const { return m_videoStream != nullptr; }
    bool isVideoEOF() const;

    QVideoFrame outputVideoFrame() const;
    QAudioBuffer outputAudioBuffer() const;

signals:
    void errorMessage(const QString& message);

private:
    Q_DISABLE_COPY(Decoder);

    bool pullFrameFromSink(Stream* stream, bool& gotFrame);
    bool sendPacketToDecoder(Stream* stream, AVPacket* packet);
    bool filter(Stream* stream, bool& gotFrame);

    bool m_formatEOF = false;
    std::unique_ptr<AVFormatContext, CloseFormatContext> m_formatContext;
    std::unique_ptr<AudioStream> m_audioStream;
    std::unique_ptr<VideoStream> m_videoStream;
    std::unique_ptr<AVPacket, FreePacket> m_packet;
};