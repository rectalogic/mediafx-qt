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

#include "image_clip.h"
#include "mediafx.h"
#include "session.h"
#include "visual_clip.h"
#include <QImage>
#include <QMessageLogContext>
#include <QUrl>
#include <QVideoFrameFormat>
#include <string.h>
struct Interval;

void ImageClip::loadMedia(const QUrl& url)
{
    auto session = MediaFX::singletonInstance()->session();
    QImage image;
    if (!url.isLocalFile() || !image.load(url.toLocalFile())) {
        qCritical("ImageClip can only load local file urls");
        emit session->exitApp(1);
        return;
    }
    videoFrame = QVideoFrame(QVideoFrameFormat(image.size(), QVideoFrameFormat::Format_RGBA8888));
    if (image.format() != QImage::Format_RGBA8888) {
        image.convertTo(QImage::Format_RGBA8888);
    }
    if (!videoFrame.map(QVideoFrame::MapMode::WriteOnly)) {
        qCritical("ImageClip can not convert image");
        emit session->exitApp(1);
        return;
    }
    memcpy(videoFrame.bits(0), image.constBits(), videoFrame.mappedBytes(0));
    videoFrame.unmap();
}

bool ImageClip::prepareNextVideoFrame(const Interval& globalTime)
{
    setCurrentVideoFrame(videoFrame);
    return true;
}

void ImageClip::stop()
{
    VisualClip::stop();
    videoFrame = QVideoFrame();
}

void ImageClip::componentComplete()
{
    loadMedia(source());
    VisualClip::componentComplete();
}