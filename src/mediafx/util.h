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

#include <QSize>
#include <QtTypes>
#include <stdio.h>

qint64 computeFrameDuration(const QString& frameRate)
{
    auto frameRateF = frameRate.toDouble();
    if (frameRateF <= 0) {
        float num = 0, den = 0;
        if (sscanf(frameRate.toUtf8().data(), "%f/%f", &num, &den) == 2 && den > 0) {
            frameRateF = num / den;
        }
    }
    qint64 frameDuration = 0;
    if (frameRateF > 0) {
        return 1000000 / frameRateF; // frame duration in microseconds
    } else {
        return -1;
    }
}

QSize parseFrameSize(const QString& frameSize)
{
    QSize size;
    if (sscanf(frameSize.toUtf8().data(), "%dx%d", &size.rwidth(), &size.rheight()) == 2 && size.width() > 0 && size.height() > 0) {
        return size;
    }
    return QSize();
}