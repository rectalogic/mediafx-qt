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

#include "visual_clip.h"
#include <QImage>
#include <QObject>
#include <QVideoFrame>
#include <QtQmlIntegration>
class QUrl;

class ImageClip : public VisualClip {
    Q_OBJECT
    QML_ELEMENT

public:
    using VisualClip::VisualClip;

protected:
    void loadMedia(const QUrl&) override;

    bool prepareNextVideoFrame() override;

    void setActive(bool active) override;

    void stop() override;

private:
    QImage image;
    QVideoFrame videoFrame;
};