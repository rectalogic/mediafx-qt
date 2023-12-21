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

#include <QObject>
#include <QtQmlIntegration>
class QVideoSink;
class MediaClip;

class VideoSinkAttached : public QObject {
    Q_OBJECT
    Q_PROPERTY(MediaClip* clip READ clip WRITE setClip NOTIFY clipChanged)
    QML_ANONYMOUS

public:
    explicit VideoSinkAttached(QVideoSink* videoSink, QObject* parent = nullptr)
        : QObject(parent)
        , m_videoSink(videoSink) {};
    MediaClip* clip() const { return m_clip; };
    void setClip(MediaClip* clip);

signals:
    void clipChanged();

private:
    MediaClip* m_clip = nullptr;
    QVideoSink* m_videoSink;
};

class VideoSink : public QObject {
    Q_OBJECT
    QML_ATTACHED(VideoSinkAttached)
    QML_ELEMENT

public:
    explicit VideoSink(QObject* parent = nullptr)
        : QObject(parent) {};
    static VideoSinkAttached* qmlAttachedProperties(QObject* object);
};