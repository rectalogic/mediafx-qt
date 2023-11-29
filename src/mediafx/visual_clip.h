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
#include <QList>
#include <QObject>
#include <QVideoFrame>
#include <QtQmlIntegration>
class QVideoSink;
struct Interval;

class VisualClip : public Clip {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("VisualClip is an abstract base class.")

public:
    using Clip::Clip;
    explicit VisualClip(QObject* parent = nullptr)
        : Clip(parent) {};

protected:
    QList<QVideoSink*> videoSinks() const { return m_videoSinks; };
    void setVideoSinks(const QList<QVideoSink*>&);

    bool renderClip(const Interval& globalTime) override;
    virtual bool prepareNextVideoFrame(const Interval& globalTime) = 0;
    QVideoFrame currentVideoFrame() const { return m_currentVideoFrame; };
    void setCurrentVideoFrame(const QVideoFrame& videoFrame) { m_currentVideoFrame = videoFrame; };

    friend class VisualSinkAttached;

private:
    QList<QVideoSink*> m_videoSinks;
    QVideoFrame m_currentVideoFrame;
};

class VisualSinkAttached : public QObject {
    Q_OBJECT
    Q_PROPERTY(VisualClip* clip READ clip WRITE setClip NOTIFY clipChanged)
    QML_ANONYMOUS

public:
    explicit VisualSinkAttached(QVideoSink* videoSink, QObject* parent = nullptr)
        : QObject(parent)
        , m_videoSink(videoSink) {};
    VisualClip* clip() const { return m_clip; };
    void setClip(VisualClip* clip);

signals:
    void clipChanged();

private:
    VisualClip* m_clip = nullptr;
    QVideoSink* m_videoSink;
};

class VisualSink : public QObject {
    Q_OBJECT
    QML_ATTACHED(VisualSinkAttached)
    QML_ELEMENT

public:
    explicit VisualSink(QObject* parent = nullptr)
        : QObject(parent) {};
    static VisualSinkAttached* qmlAttachedProperties(QObject* object);
};