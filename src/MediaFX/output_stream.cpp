// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * mux raw audio (float) and video (RGBA) streams into NUT format
 * We bypass encoding and just stuff our raw data into AVPacket.data for
 * the muxer since it is already in the correct format.
 * http://git.ffmpeg.org/gitweb/nut.git
 * https://ffmpeg.org/nut.html
 */

#include "output_stream.h"
#include "util.h"
#include <QDebug>
#include <QtLogging>
#include <stdint.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

// NOLINTBEGIN(bugprone-assignment-in-if-condition)

OutputStream::OutputStream(AVFormatContext* formatContext, enum AVCodecID codecID)
    : m_codec(avcodec_find_encoder(codecID))
{
    if (!m_codec) {
        qCritical() << "Could not find encoder for" << avcodec_get_name(codecID);
        return;
    }

    m_packet = av_packet_alloc();
    if (!m_packet) {
        qCritical() << "Could not allocate AVPacket, av_packet_alloc";
        return;
    }

    m_stream = avformat_new_stream(formatContext, nullptr);
    if (!m_stream) {
        qCritical() << "Could not allocate stream, avformat_new_stream";
        return;
    }
    m_stream->id = static_cast<int>(formatContext->nb_streams - 1);
    m_codecContext = avcodec_alloc_context3(m_codec);
    if (!m_codecContext) {
        qCritical() << "Could not allocate an encoding context, avcodec_alloc_context3";
        return;
    }
    m_isValid = true;
}

OutputStream::~OutputStream()
{
    if (m_codecContext)
        avcodec_free_context(&m_codecContext);
    if (m_packet)
        av_packet_free(&m_packet);
};

bool OutputStream::open()
{
    int ret = avcodec_open2(m_codecContext, m_codec, nullptr);
    if (ret < 0) {
        qCritical() << "Could not open cocdec, avcodec_open2: " << av_err2qstring(ret);
        return false;
    }
    ret = avcodec_parameters_from_context(m_stream->codecpar, m_codecContext);
    if (ret < 0) {
        qCritical() << "Could not copy codec parameters, avcodec_parameters_from_context: " << av_err2qstring(ret);
        return false;
    }
    return true;
};

bool OutputStream::writePacket(AVFormatContext* formatContext, int64_t ptsIncrement)
{
    m_packet->pts = m_nextPTS;
    m_packet->dts = m_nextPTS;
    av_packet_rescale_ts(m_packet, m_codecContext->time_base, m_stream->time_base);
    m_packet->stream_index = m_stream->index;
    int ret = av_write_frame(formatContext, m_packet);
    if (ret < 0) {
        qCritical() << "Could not write frame, av_write_frame: " << av_err2qstring(ret);
        return false;
    }
    m_nextPTS += ptsIncrement;
    return true;
};

// NOLINTEND(bugprone-assignment-in-if-condition)