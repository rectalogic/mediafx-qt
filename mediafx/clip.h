// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QImage>
#include <QList>
#include <QMediaPlayer>
#include <QMediaTimeRange>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QUrl>
#include <QVideoFrame>
#include <QVideoSink>
#include <QtQmlIntegration>
#include <QtTypes>
class QVideoSink;

class Clip : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged FINAL)
    Q_PROPERTY(qint64 clipEnd READ clipEnd WRITE setClipEnd NOTIFY clipEndChanged FINAL)
    Q_PROPERTY(QList<QVideoSink*> videoSinks READ videoSinks WRITE setVideoSinks FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Clip is an abstract base class.")

public:
    using QObject::QObject;

    Clip()
        : QObject()
        , m_clipTimeRange(0, -1)
        , m_currentTimelineTimeRange(-1, -1)
        , m_nextFrameTimeRange(0, -1)
    {
    }

    QUrl url() const { return m_url; };
    void setUrl(const QUrl&);

    virtual qint64 duration() const = 0;

    qint64 clipStart() const { return m_clipTimeRange.start(); };
    void setClipStart(qint64);

    qint64 clipEnd() const { return m_clipTimeRange.end(); };
    void setClipEnd(qint64);

    QList<QVideoSink*> videoSinks() const { return m_videoSinks; };
    void setVideoSinks(const QList<QVideoSink*>&);

    bool renderVideoFrame(QMediaTimeRange::Interval& timelineFrameTimeRange);
    virtual bool prepareNextVideoFrame() = 0;

    virtual void setActive(bool active);

protected:
    virtual void loadMedia(const QUrl&) = 0;

    QMediaTimeRange::Interval nextFrameTimeRange() const { return m_nextFrameTimeRange; };
    void setNextFrameTimeRange(const QMediaTimeRange::Interval& timeRange) { m_nextFrameTimeRange = timeRange; };

    QMediaTimeRange::Interval clipTimeRange() const { return m_clipTimeRange; };

    QMediaTimeRange::Interval currentTimelineTimeRange() const { return m_currentTimelineTimeRange; };
    void setCurrentTimelineTimeRange(const QMediaTimeRange::Interval& currentTime) { m_currentTimelineTimeRange = currentTime; };

    void setCurrentVideoFrame(const QVideoFrame& videoFrame) { m_currentVideoFrame = videoFrame; };

signals:
    void clipStartChanged();
    void clipEndChanged();
    void durationChanged();

private:
    QUrl m_url;
    QMediaTimeRange::Interval m_clipTimeRange;
    QMediaTimeRange::Interval m_currentTimelineTimeRange;
    QMediaTimeRange::Interval m_nextFrameTimeRange;
    QList<QVideoSink*> m_videoSinks = nullptr;
    QVideoFrame m_currentVideoFrame;
};

/******************************/

class MediaClip : public Clip {
    Q_OBJECT
    QML_ELEMENT

public:
    using Clip::Clip;
    MediaClip();

    qint64 duration() const override;

    bool prepareNextVideoFrame() override;

    void setActive(bool active) override;

protected:
    void loadMedia(const QUrl&) override;

private slots:
    void onErrorOccurred(QMediaPlayer::Error error, const QString& errorString);
    void onVideoFrameChanged(const QVideoFrame& frame);
    void onDurationChanged(qint64 duration);

private:
    void rateControl();
    static const int MaxFrameQueueSize = 20;
    static const int MinFrameQueueSize = 5;
    QMediaPlayer mediaPlayer;
    QVideoSink mediaPlayerSink;
    QQueue<QVideoFrame> frameQueue;
};

/******************************/

class ImageClip : public Clip {
    Q_OBJECT
    QML_ELEMENT

public:
    using Clip::Clip;

    qint64 duration() const override;

    bool prepareNextVideoFrame() override;

protected:
    void loadMedia(const QUrl&) override;

private:
    QImage image;
    QVideoFrame videoFrame;
};