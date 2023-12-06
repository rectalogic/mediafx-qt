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

#pragma once

#include <QElapsedTimer>
#include <QQueue>
#include <QVideoFrame>
#include <QtTypes>
#include <chrono>
using namespace std::chrono;
using namespace std::chrono_literals;

class RateControl {
public:
    explicit RateControl(const microseconds& targetFrameDuration)
        : targetFrameDuration(targetFrameDuration) {};

    void enqueue(const QVideoFrame& videoFrame);
    QVideoFrame dequeue() { return bufferedFrames.dequeue(); };
    void onVideoFrameRequired();
    bool isEmpty() const noexcept { return bufferedFrames.isEmpty(); };
    qsizetype size() const noexcept { return bufferedFrames.size(); };
    qreal playbackRate() const;
    void reset();

private:
    QElapsedTimer frameRequiredTimer;
    nanoseconds timeSinceFrameLastRequired = nanoseconds::zero();
    bool isFrameRequiredTimerValid = false;
    QQueue<QVideoFrame> bufferedFrames;
    qreal rateScalar = 1.0;
    microseconds targetFrameDuration;
    // If we can't determine source frame duration, just assume 30fps
    nanoseconds sourceFrameDuration = 33333333ns;
};