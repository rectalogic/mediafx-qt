// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioFormat>
#include <QString>
#include <ffms.h>
using namespace Qt::Literals::StringLiterals;

inline constexpr int64_t AudioFFMSChannelLayout = FFMS_CH_FRONT_LEFT | FFMS_CH_FRONT_RIGHT;
inline constexpr auto AudioQtChannelConfig = QAudioFormat::ChannelConfigStereo;
inline constexpr FFMS_SampleFormat AudioFFMSSampleFormat = FFMS_FMT_FLT;
inline constexpr auto AudioQtSampleFormat = QAudioFormat::Float;

inline constexpr auto AudioFFMPEGChannelLayout = "stereo";
inline constexpr auto AudioFFMPEGChannelCount = "2";
inline constexpr auto AudioFFMPEGSampleFormat =
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    "f32be";
#else
    "f32le";
#endif
