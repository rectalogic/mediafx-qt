// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
extern "C" {
#include <libavcodec/codec_id.h>
}
class AVStream;
class AVCodec;
class AVCodecContext;
class AVFormatContext;
class AVPacket;

class OutputStream {
public:
    explicit OutputStream(AVFormatContext* formatContext, enum AVCodecID codecID);
    OutputStream(OutputStream&&) = delete;
    OutputStream(const OutputStream&) = delete;
    OutputStream& operator=(OutputStream&&) = delete;
    OutputStream& operator=(const OutputStream&) = delete;
    ~OutputStream();

    bool isValid() const { return m_isValid; }

    bool open();
    bool writePacket(AVFormatContext* formatContext, int64_t ptsIncrement);

    const AVCodec* codec() const { return m_codec; };
    AVCodecContext* codecContext() const { return m_codecContext; };
    AVStream* stream() const { return m_stream; };
    AVPacket* packet() const { return m_packet; };

private:
    bool m_isValid = false;
    AVStream* m_stream = nullptr;
    const AVCodec* m_codec = nullptr;
    AVCodecContext* m_codecContext = nullptr;
    AVPacket* m_packet = nullptr;
    int64_t m_nextPTS = 0;
};
