// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "visual_clip.h"
#include "mediafx.h"
#include "session.h"
#include "video_clip.h"
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QQmlEngine>
#include <QUrl>
#include <QVideoSink>
#include <QmlTypeAndRevisionsRegistration>

void VisualClip::setVideoSinks(const QList<QVideoSink*>& videoSinks)
{
    if (m_videoSinks != videoSinks) {
        m_videoSinks = videoSinks;
        setActive(!videoSinks.isEmpty());
    }
}

bool VisualClip::active()
{
    return !m_videoSinks.isEmpty();
}

void VisualClip::setActive(bool active)
{
    Clip::setActive(active);
    if (!active) {
        m_videoSinks.clear();
    }
}

bool VisualClip::renderClip(const QMediaTimeRange::Interval& globalTime)
{
    if (!prepareNextVideoFrame())
        return false;
    for (auto videoSink : videoSinks()) {
        videoSink->setVideoFrame(m_currentVideoFrame);
    }
    return true;
}

void VisualClip::stop()
{
    Clip::stop();
    setVideoSinks(QList<QVideoSink*>());
}