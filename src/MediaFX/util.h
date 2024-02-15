// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <array>
#include <chrono>
extern "C" {
#include <libavutil/error.h>
#include <libavutil/rational.h>
}
using namespace std::chrono;

inline microseconds frameRateToDuration(const AVRational& frameRate)
{
    return round<microseconds>(duration<double>(av_q2d(av_inv_q(frameRate))));
}

av_always_inline QString av_err2qstring(int errnum)
{
    std::array<char, AV_ERROR_MAX_STRING_SIZE> str; // NOLINT(cppcoreguidelines-pro-type-member-init)
    return av_make_error_string(str.data(), AV_ERROR_MAX_STRING_SIZE, errnum);
}