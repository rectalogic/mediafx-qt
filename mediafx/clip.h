// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QImage>
#include <QMediaPlayer>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtQmlIntegration>
#include <QtTypes>
class QVideoSink;

class Clip : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 clipEnd READ clipEnd WRITE setClipEnd NOTIFY clipEndChanged FINAL)
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Clip is an abstract base class.")
public:
    using QObject::QObject;
    QUrl url() const { return m_url; };
    void setUrl(const QUrl&);
    virtual qint64 duration() const = 0;
    qint64 clipEnd() const { return m_clipEnd; };
    void setClipEnd(qint64);
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink*);

protected:
    virtual void loadMedia(const QUrl&) = 0;

signals:
    void clipEndChanged();
    void durationChanged();

private:
    QUrl m_url;
    qint64 m_clipEnd = -1;
    QVideoSink* m_videoSink = nullptr;
};

class MediaClip : public Clip {
    Q_OBJECT
    Q_PROPERTY(qint64 clipBegin READ clipBegin WRITE setClipBegin NOTIFY clipBeginChanged FINAL)
    QML_ELEMENT
public:
    using Clip::Clip;
    MediaClip();
    qint64 duration() const override;
    qint64 clipBegin() const { return m_clipBegin; };
    void setClipBegin(qint64);

protected:
    void loadMedia(const QUrl&) override;

signals:
    void clipBeginChanged();

private slots:
    void onErrorOccurred(QMediaPlayer::Error error, const QString& errorString);

private:
    QMediaPlayer mediaPlayer;
    qint64 m_clipBegin = 0;
};

class ImageClip : public Clip {
    Q_OBJECT
    QML_ELEMENT
public:
    using Clip::Clip;
    qint64 duration() const override;

protected:
    void loadMedia(const QUrl&) override;

private:
    QImage image;
};