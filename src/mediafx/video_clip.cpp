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
#include <QMediaPlayer>
#include <QMessageLogContext>
#include <QMetaObject>
#include <QMutexLocker>
#include <QUrl>
#include <QVideoSink>
#include <Qt>
#include <chrono>
#include <compare>
#include <type_traits>
using namespace std::chrono;
using namespace std::chrono_literals;

VideoClip::VideoClip(QObject* parent)
    : VisualClip(parent)
    , rateControl(MediaFX::singletonInstance()->session()->frameDuration())
{
    connect(&mediaPlayerSink, &QVideoSink::videoFrameChanged, this, &VideoClip::onVideoFrameChanged, Qt::DirectConnection);
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

void VideoClip::onRateControl()
{
    auto rate = rateControl.playbackRate();
    mediaPlayer.setPlaybackRate(rate);
}

void VideoClip::onVideoFrameChanged(const QVideoFrame& frame)
{
    QMutexLocker locker(&mutex);
    auto frameTimeStart = nextClipTime().start();
    if (microseconds(frame.endTime()) < frameTimeStart) {
        return;
    }
    if (microseconds(frame.startTime()) >= frameTimeStart) {
        rateControl.enqueue(frame);
        QMetaObject::invokeMethod(this, &VideoClip::onRateControl, Qt::QueuedConnection);
    }
}

bool VideoClip::prepareNextVideoFrame(const Interval& globalTime)
{
    QMutexLocker locker(&mutex);
    auto nextFrameTime = nextClipTime();
    // clipEnd may be beyond duration, if so we just keep last frame rendered
    if (nextFrameTime.start() >= milliseconds(mediaPlayer.duration())) {
        return true;
    }
    QVideoFrame videoFrame = currentVideoFrame();
    if (videoFrame.isValid() && nextFrameTime.contains(microseconds(videoFrame.startTime()))) {
        return true;
    }

    if (lastRequestedGlobalTime != globalTime) {
        rateControl.onVideoFrameRequired();
        lastRequestedGlobalTime = globalTime;
    }
    while (!rateControl.isEmpty()) {
        QVideoFrame frame = rateControl.dequeue();
        if (nextFrameTime.contains(microseconds(frame.startTime()))) {
            setCurrentVideoFrame(frame);
            onRateControl();
            return true;
        }
    }

    // No buffered frames and state is ended
    if (mediaPlayer.mediaStatus() == QMediaPlayer::EndOfMedia) {
        stop();
        return false;
    }
    onRateControl();
    return false;
}

void VideoClip::onActiveChanged(bool active)
{
    VisualClip::onActiveChanged(active);
    if (isComponentComplete()) {
        if (active) {
            play();
        } else {
            mediaPlayer.pause();
        }
    }
}

void VideoClip::play()
{
    bool initialPlay = mediaPlayer.playbackState() == QMediaPlayer::StoppedState;
    mediaPlayer.play();
    if (initialPlay && clipStart() > 0 && mediaPlayer.isSeekable()) {
        mediaPlayer.setPosition(clipStart());
    }
}

void VideoClip::stop()
{
    VisualClip::stop();
    mediaPlayer.stop();
    mediaPlayer.setSource(QUrl());
    rateControl.reset();
}

void VideoClip::componentComplete()
{
    loadMedia(source());
    if (clipEnd_us() == -1us) {
        setClipEnd_us(milliseconds(mediaPlayer.duration()));
    }
    VisualClip::componentComplete();
    if (isActive())
        play();
}