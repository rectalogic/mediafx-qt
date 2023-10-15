// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include "mediafx.h"
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QQmlEngine>
#include <QUrl>
#include <QVideoSink>

void Clip::setUrl(const QUrl& url)
{
    if (!m_url.isEmpty()) {
        qWarning() << "Clip url is a write-once property and cannot be changed";
        return;
    }
    m_url = url;
}

void Clip::setVideoSinks(const QList<QVideoSink*>& videoSinks)
{
    if (m_videoSinks != videoSinks) {
        m_videoSinks = videoSinks;
        setActive(!videoSinks.isEmpty());
    }
}

void Clip::setClipStart(qint64 clipStart)
{
    if (m_clipTimeRange.start() != clipStart) {
        m_clipTimeRange = QMediaTimeRange::Interval(clipStart, m_clipTimeRange.end());
        // We don't know frame duration yet
        setNextFrameTimeRange(QMediaTimeRange::Interval(clipStart, -1));
        emit clipStartChanged();
        emit durationChanged();
    }
}

void Clip::setClipEnd(qint64 clipEnd)
{
    if (m_clipTimeRange.end() != clipEnd) {
        m_clipTimeRange = QMediaTimeRange::Interval(m_clipTimeRange.start(), clipEnd);
        emit clipEndChanged();
        emit durationChanged();
    }
}

void Clip::setActive(bool active)
{
    auto mediaFX = qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId);
    if (active) {
        mediaFX->registerClip(this);
    } else {
        mediaFX->unregisterClip(this);
        m_videoSinks.clear();
    }
}

bool Clip::renderVideoFrame(const QMediaTimeRange::Interval& timelineFrameTimeRange)
{
    if (currentTimelineTimeRange() == timelineFrameTimeRange)
        return true;
    qint64 duration = timelineFrameTimeRange.end() - timelineFrameTimeRange.start();
    if (nextFrameTimeRange().end() == -1) {
        setNextFrameTimeRange(QMediaTimeRange::Interval(clipStart(), clipStart() + duration));
    }
    if (!videoSinks().isEmpty() && clipTimeRange().contains(nextFrameTimeRange().start())) {
        if (!prepareNextVideoFrame())
            return false;
        m_currentVideoFrame.setStartTime(timelineFrameTimeRange.start());
        m_currentVideoFrame.setEndTime(timelineFrameTimeRange.end());
        for (auto videoSink : videoSinks()) {
            videoSink->setVideoFrame(m_currentVideoFrame);
        }
        setCurrentTimelineTimeRange(timelineFrameTimeRange);
        setNextFrameTimeRange(nextFrameTimeRange().translated(duration));
        return true;
    } else {
        setActive(false);
        return false;
    }
}

/******************************/

MediaClip::MediaClip()
    : Clip()
{
    // We want frames faster than realtime
    mediaPlayer.setPlaybackRate(1000);
    connect(&mediaPlayerSink, &QVideoSink::videoFrameChanged, this, &MediaClip::onVideoFrameChanged);
    mediaPlayer.setVideoSink(&mediaPlayerSink);
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &MediaClip::onDurationChanged);
    connect(&mediaPlayer, &QMediaPlayer::errorOccurred, this, &MediaClip::onErrorOccurred);
}

void MediaClip::onErrorOccurred(QMediaPlayer::Error error, const QString& errorString)
{
    qCritical() << "MediaClip error " << error << " " << errorString;
    QCoreApplication::exit(1);
}

void MediaClip::loadMedia(const QUrl& url)
{
    mediaPlayer.setSource(url);
}

void MediaClip::onDurationChanged(qint64 duration)
{
    setClipEnd(duration); // This will emit durationChanged as well
}

qint64 MediaClip::duration() const
{
    if (clipEnd() != -1)
        return clipEnd() - clipStart();
    else {
        return mediaPlayer.duration() - clipStart();
    }
}

void MediaClip::rateControl()
{
    auto size = frameQueue.size();
    if (size > MaxFrameQueueSize && mediaPlayer.isPlaying())
        mediaPlayer.pause();
    else if (size < MinFrameQueueSize && !mediaPlayer.isPlaying())
        mediaPlayer.play();
}

void MediaClip::onVideoFrameChanged(const QVideoFrame& frame)
{
    if (videoSinks().isEmpty())
        return;

    // XXX need to attempt to seek - should we do that when clipStart set, or when url set? we don't know which is set first

    auto frameTime = nextFrameTimeRange();
    if (frame.endTime() >= frameTime.end())
        return;
    if (frame.startTime() >= frameTime.start()) {
        frameQueue.enqueue(frame);
        rateControl();
    }
}

bool MediaClip::prepareNextVideoFrame()
{
    QVideoFrame videoFrame;
    while (!frameQueue.isEmpty()) {
        videoFrame = frameQueue.dequeue();
        if (nextFrameTimeRange().contains(videoFrame.startTime())) {
            setCurrentVideoFrame(videoFrame);
            rateControl();
            return true;
        }
    }
    rateControl();
    return false;
}

void MediaClip::setActive(bool active)
{
    Clip::setActive(active);
    if (active) {
        mediaPlayer.play();
    } else {
        mediaPlayer.stop(); // XXX if we stop we should set source to QUrl(), but maybe we should pause (and if so, don't clear queue)? could clip be resumed later?
        frameQueue.clear();
        frameQueue.squeeze();
    }
}

/******************************/

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
