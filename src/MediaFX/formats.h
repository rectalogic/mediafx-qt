// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioFormat>
#include <QString>
#include <QVideoFrameFormat>
#include <ffms.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/pixfmt.h>
}
using namespace Qt::Literals::StringLiterals;

inline constexpr int64_t AudioChannelLayout_FFMS2 = FFMS_CH_FRONT_LEFT | FFMS_CH_FRONT_RIGHT;
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(57, 28, 100)
inline constexpr int64_t AudioChannelLayout_FFMPEG = AV_CH_LAYOUT_STEREO;
#else
inline constexpr AVChannelLayout AudioChannelLayout_FFMPEG = AV_CHANNEL_LAYOUT_STEREO;
#endif
inline constexpr auto AudioChannelLayout_Qt = QAudioFormat::ChannelConfigStereo;

inline constexpr FFMS_SampleFormat AudioSampleFormat_FFMS2 = FFMS_FMT_FLT;
inline constexpr auto AudioSampleFormat_Qt = QAudioFormat::Float;
inline constexpr auto AudioSampleFormat_FFMPEG = AV_SAMPLE_FMT_FLT;
inline constexpr auto AudioCodec_FFMPEG =
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    AV_CODEC_ID_PCM_F32BE;
#else
    AV_CODEC_ID_PCM_F32LE;
#endif

inline constexpr AVPixelFormat VideoPixelFormat_FFMPEG = AV_PIX_FMT_RGBA;
inline constexpr QVideoFrameFormat::PixelFormat VideoPixelFormat_Qt = QVideoFrameFormat::Format_RGBA8888;