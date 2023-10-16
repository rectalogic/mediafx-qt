// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "image_clip.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QUrl>
#include <QVideoFrameFormat>
#include <string.h>

void ImageClip::loadMedia(const QUrl& url)
{
    QImage image;
    if (!url.isLocalFile() || !image.load(url.toLocalFile())) {
        qCritical("ImageClip can only load local file urls");
        QCoreApplication::exit(1);
        return;
    }
    videoFrame = QVideoFrame(QVideoFrameFormat(image.size(), QVideoFrameFormat::Format_RGBA8888));
    if (image.format() != QImage::Format_RGBA8888) {
        image.convertTo(QImage::Format_RGBA8888);
    }
    if (!videoFrame.map(QVideoFrame::MapMode::WriteOnly)) {
        qCritical("ImageClip can not convert image");
        QCoreApplication::exit(1);
        return;
    }
    memcpy(videoFrame.bits(0), image.constBits(), videoFrame.mappedBytes(0));
    videoFrame.unmap();
}

qint64 ImageClip::duration() const
{
    auto ce = clipEnd();
    // XXX can we make this property required in the subclass?
    if (ce == -1)
        qCritical() << "ImageClip must have clipEnd set";
    return ce;
}

bool ImageClip::prepareNextVideoFrame()
{
    setCurrentVideoFrame(videoFrame);
    return true;
}
