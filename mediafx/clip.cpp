// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include "mediafx.h"
#include <QDebug>
#include <QList>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QQmlEngine>
#include <QUrl>
#include <QVideoSink>
#include <QmlTypeAndRevisionsRegistration>

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
