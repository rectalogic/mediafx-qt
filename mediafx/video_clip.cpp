// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "video_clip.h"
#include "mediafx.h"
#include "session.h"
#include "visual_clip.h"
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QQmlEngine>
#include <QUrl>
#include <QVideoSink>

VideoClip::VideoClip()
    : VisualClip()
{
    // We want frames faster than realtime
    mediaPlayer.setPlaybackRate(1000);
    connect(&mediaPlayerSink, &QVideoSink::videoFrameChanged, this, &VideoClip::onVideoFrameChanged);
    mediaPlayer.setVideoSink(&mediaPlayerSink);
    connect(&mediaPlayer, &QMediaPlayer::errorOccurred, this, &VideoClip::onMediaPlayerErrorOccurred);
}

void VideoClip::onMediaPlayerErrorOccurred(QMediaPlayer::Error error, const QString& errorString)
{
    qCritical() << "VideoClip error " << error << " " << errorString;
    emit MediaFX::singletonInstance()->session()->exitApp(1);
}

void VideoClip::loadMedia(const QUrl& url)
{
    mediaPlayer.setSource(url);
}

void VideoClip::rateControl()
{
    auto size = bufferedFrames.size();
    if (size > MaxFrameQueueSize && mediaPlayer.isPlaying())
        mediaPlayer.pause();
    else if (size < MinFrameQueueSize && !mediaPlayer.isPlaying())
        mediaPlayer.play();
}

void VideoClip::onVideoFrameChanged(const QVideoFrame& frame)
{
    if (videoSinks().isEmpty())
        return;

    // XXX need to attempt to seek - should we do that when clipStart set, or when url set? we don't know which is set first
    // XXX seek doesn't work until we are playing, so need to do it here

    auto frameTimeStart = nextClipTime().start();
    if (frame.endTime() < frameTimeStart) {
        return;
    }
    if (frame.startTime() >= frameTimeStart) {
        bufferedFrames.enqueue(frame);
        rateControl();
    }
}

bool VideoClip::prepareNextVideoFrame()
{
    auto nextFrameTime = nextClipTime();
    // clipEnd may be beyond duration, if so we just keep last frame rendered
    if (nextFrameTime.start() >= mediaPlayer.duration() * 1000) {
        return true;
    }
    QVideoFrame videoFrame = currentVideoFrame();
    if (videoFrame.isValid() && nextFrameTime.contains(videoFrame.startTime())) {
        return true;
    }
    while (!bufferedFrames.isEmpty()) {
        videoFrame = bufferedFrames.dequeue();
        if (nextFrameTime.contains(videoFrame.startTime())) {
            setCurrentVideoFrame(videoFrame);
            rateControl();
            return true;
        }
    }
    rateControl();
    return false;
}

void VideoClip::setActive(bool active)
{
    VisualClip::setActive(active);
    if (isComponentComplete()) {
        if (active) {
            if (mediaPlayer.source().isEmpty())
                loadMedia(source());
            mediaPlayer.play();
        } else {
            mediaPlayer.pause();
        }
    }
}

void VideoClip::stop()
{
    VisualClip::stop();
    mediaPlayer.stop();
    mediaPlayer.setSource(QUrl());
    bufferedFrames.clear();
    bufferedFrames.squeeze();
}

void VideoClip::componentComplete()
{
    loadMedia(source());
    if (clipEndMicros() == -1) {
        setClipEndMicros(mediaPlayer.duration() * 1000);
    }
    VisualClip::componentComplete();
}