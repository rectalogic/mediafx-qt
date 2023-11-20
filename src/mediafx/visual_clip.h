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

#include "clip.h"
#include "interval.h"
#include <QList>
#include <QObject>
#include <QVideoFrame>
#include <QtQmlIntegration>
class QVideoSink;

class VisualClip : public Clip {
    Q_OBJECT
    Q_PROPERTY(QList<QVideoSink*> videoSinks READ videoSinks WRITE setVideoSinks FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("VisualClip is an abstract base class.")

public:
    using Clip::Clip;
    explicit VisualClip(QObject* parent = nullptr)
        : Clip(parent) {};

    QList<QVideoSink*> videoSinks() const { return m_videoSinks; };
    void setVideoSinks(const QList<QVideoSink*>&);

protected:
    bool renderClip(const Interval& globalTime) override;
    virtual bool prepareNextVideoFrame() = 0;
    QVideoFrame currentVideoFrame() const { return m_currentVideoFrame; };
    void setCurrentVideoFrame(const QVideoFrame& videoFrame) { m_currentVideoFrame = videoFrame; };

private:
    QList<QVideoSink*> m_videoSinks;
    QVideoFrame m_currentVideoFrame;
};
