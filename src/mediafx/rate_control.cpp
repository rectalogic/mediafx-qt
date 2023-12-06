/*
 * Copyright (C) 2023 Andrew Wason
 *
 * This file is part of mediaFX.
 *
 * mediaFX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with mediaFX.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "rate_control.h"
#include <chrono>
#include <ratio>
using namespace std::chrono;

const int desiredBufferSize = 3;

qreal RateControl::playbackRate() const
{
    qreal rate = desiredBufferSize * rateScalar;
    if (timeSinceFrameLastRequired != nanoseconds::zero()) {
        rate = (sourceFrameDuration / duration<qreal, nanoseconds::period>(timeSinceFrameLastRequired))
            * rateScalar
            * (bufferedFrames.isEmpty() ? desiredBufferSize : (desiredBufferSize / (qreal)bufferedFrames.size()));
    }
    return rate;
}

void RateControl::onVideoFrameRequired()
{
    if (frameRequiredTimer.isValid())
        timeSinceFrameLastRequired = frameRequiredTimer.durationElapsed();
    frameRequiredTimer.start();
}

void RateControl::enqueue(const QVideoFrame& videoFrame)
{
    if (videoFrame.startTime() != -1 && videoFrame.endTime() != -1) {
        microseconds frameDuration(videoFrame.endTime() - videoFrame.startTime());
        rateScalar = targetFrameDuration / duration<double, microseconds::period>(frameDuration);
        sourceFrameDuration = frameDuration;
    }
    bufferedFrames.enqueue(videoFrame);
}

void RateControl::reset()
{
    bufferedFrames.clear();
    bufferedFrames.squeeze();
    frameRequiredTimer.invalidate();
    timeSinceFrameLastRequired = nanoseconds::zero();
}