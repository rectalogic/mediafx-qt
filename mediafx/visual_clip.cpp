// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "visual_clip.h"
#include "mediafx.h"
#include <QList>
#include <QMediaTimeRange>
#include <QQmlEngine>
#include <QVideoSink>
#include <QmlTypeAndRevisionsRegistration>
#include <QtTypes>

void VisualClip::setVideoSinks(const QList<QVideoSink*>& videoSinks)
{
    if (m_videoSinks != videoSinks) {
        m_videoSinks = videoSinks;
        setActive(!videoSinks.isEmpty());
    }
}

void VisualClip::setActive(bool active)
{
    auto mediaFX = qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId);
    if (active) {
        mediaFX->registerVisualClip(this);
    } else {
        mediaFX->unregisterVisualClip(this);
        m_videoSinks.clear();
    }
}

bool VisualClip::renderVideoFrame(const QMediaTimeRange::Interval& globalTime)
{
    if (currentGlobalTime() == globalTime)
        return true;
    qint64 duration = globalTime.end() - globalTime.start();
    if (nextClipTime().end() == -1) {
        setNextClipTime(QMediaTimeRange::Interval(clipStart(), clipStart() + duration));
    }
    if (!videoSinks().isEmpty() && clipTimeRange().contains(nextClipTime().start())) {
        if (!prepareNextVideoFrame())
            return false;
        m_currentVideoFrame.setStartTime(globalTime.start());
        m_currentVideoFrame.setEndTime(globalTime.end());
        for (auto videoSink : videoSinks()) {
            videoSink->setVideoFrame(m_currentVideoFrame);
        }
        setCurrentGlobalTime(globalTime);
        setNextClipTime(nextClipTime().translated(duration));
        return true;
    } else if (clipEnd() < nextClipTime().start()) {
        stop();
        return false;
    } else {
        setActive(false);
        return false;
    }
}

void VisualClip::stop()
{
    Clip::stop();
    setVideoSinks(QList<QVideoSink*>());
}