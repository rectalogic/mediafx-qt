// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "video_clip.h"
#include "visual_clip.h"
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QMediaTimeRange>
#include <QMessageLogContext>
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
    QCoreApplication::exit(1);
}

void VideoClip::loadMedia(const QUrl& url)
{
    mediaPlayer.setSource(url);
}

qint64 VideoClip::duration() const
{
    if (clipEnd() != -1)
        return clipEnd() - clipStart();
    else {
        return mediaPlayer.duration() - clipStart();
    }
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

    auto frameTime = nextClipTime();
    if (frame.endTime() >= frameTime.end())
        return;
    if (frame.startTime() >= frameTime.start()) {
        bufferedFrames.enqueue(frame);
        rateControl();
    }
}

bool VideoClip::prepareNextVideoFrame()
{
    QVideoFrame videoFrame;
    while (!bufferedFrames.isEmpty()) {
        videoFrame = bufferedFrames.dequeue();
        if (nextClipTime().contains(videoFrame.startTime())) {
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
    if (active) {
        if (mediaPlayer.source().isEmpty()) {
            loadMedia(source());
        }
        mediaPlayer.play();
    } else {
        mediaPlayer.pause();
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