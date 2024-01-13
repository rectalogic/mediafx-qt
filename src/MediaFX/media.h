// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "media_clip.h" // IWYU pragma: keep
#include <QObject>
#include <QtQmlIntegration>
class QVideoSink;

class MediaAttached : public QObject {
    Q_OBJECT
    Q_PROPERTY(MediaClip* clip READ clip WRITE setClip NOTIFY clipChanged)
    QML_ANONYMOUS

public:
    explicit MediaAttached(QVideoSink* videoSink, QObject* parent = nullptr)
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

class Media : public QObject {
    Q_OBJECT
    QML_ATTACHED(MediaAttached)
    QML_ELEMENT

public:
    explicit Media(QObject* parent = nullptr)
        : QObject(parent) {};
    static MediaAttached* qmlAttachedProperties(QObject* object);
};