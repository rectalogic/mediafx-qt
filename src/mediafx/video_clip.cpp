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

#include "video_clip.h"
#include "interval.h"
#include "mediafx.h"
#include "session.h"
#include "visual_clip.h"
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QMessageLogContext>
#include <QUrl>
#include <QVideoSink>

VideoClip::VideoClip(QObject* parent)
    : VisualClip(parent)
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

void VideoClip::setActive(bool bactive)
{
    VisualClip::setActive(bactive);
    if (isComponentComplete()) {
        if (active()) {
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
    if (active())
        mediaPlayer.play();
}