// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "render_context.h"
#include <QAudioFormat>
#include <QString>
#include <QVideoFrameFormat>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/pixfmt.h>
}
using namespace Qt::Literals::StringLiterals;

#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(57, 28, 100)
inline constexpr int64_t AudioChannelLayout_FFMPEG = AV_CH_LAYOUT_STEREO;
#else
inline constexpr AVChannelLayout AudioChannelLayout_FFMPEG = AV_CHANNEL_LAYOUT_STEREO;
#endif
inline constexpr auto AudioChannelLayoutName_FFMPEG = "stereo";
inline constexpr auto AudioChannelLayout_Qt = QAudioFormat::ChannelConfigStereo;

inline constexpr auto AudioSampleFormat_Qt = QAudioFormat::Float;
inline constexpr auto AudioSampleFormat_FFMPEG = AV_SAMPLE_FMT_FLT;
inline constexpr auto AudioCodec_FFMPEG =
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    AV_CODEC_ID_PCM_F32BE;
#else
    AV_CODEC_ID_PCM_F32LE;
#endif

inline constexpr AVPixelFormat VideoPixelFormat_FFMPEG = AV_PIX_FMT_RGBA;
inline constexpr auto VideoPixelFormatName_FFMPEG = "rgba";
inline constexpr auto VideoPixelFormat_Qt = QVideoFrameFormat::Format_RGBA8888;
inline constexpr auto VideoColorSpace_Qt = QVideoFrameFormat::ColorSpace_AdobeRgb;
inline constexpr auto VideoColorSpace_FFMPEG = AVCOL_SPC_RGB;
inline constexpr auto VideoColorRange_Qt = QVideoFrameFormat::ColorRange_Full;
inline constexpr auto VideoColorRange_FFMPEG = AVCOL_RANGE_JPEG;
