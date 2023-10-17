// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "clip.h"
#include <QImage>
#include <QObject>
#include <QVideoFrame>
#include <QtQmlIntegration>
#include <QtTypes>
class QUrl;

class ImageClip : public Clip {
    Q_OBJECT
    QML_ELEMENT

public:
    using Clip::Clip;

    qint64 duration() const override;

    void stop() override;

protected:
    void loadMedia(const QUrl&) override;

    bool prepareNextVideoFrame() override;

    void setActive(bool active) override;

private:
    QImage image;
    QVideoFrame videoFrame;
};