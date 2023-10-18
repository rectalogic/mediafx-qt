// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include <QDebug>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QUrl>

void Clip::setUrl(const QUrl& url)
{
    if (!m_url.isEmpty()) {
        qWarning() << "Clip url is a write-once property and cannot be changed";
        return;
    }
    m_url = url;
}

void Clip::setClipStart(qint64 clipStart)
{
    if (m_clipSegment.start() != clipStart) {
        m_clipSegment = QMediaTimeRange::Interval(clipStart, m_clipSegment.end());
        // We don't know frame duration yet
        setNextClipTime(QMediaTimeRange::Interval(clipStart, -1));
        emit clipStartChanged();
        emit durationChanged();
    }
}

void Clip::setClipEnd(qint64 clipEnd)
{
    if (m_clipSegment.end() != clipEnd) {
        m_clipSegment = QMediaTimeRange::Interval(m_clipSegment.start(), clipEnd);
        emit clipEndChanged();
        emit durationChanged();
    }
}

bool Clip::render(const QMediaTimeRange::Interval& globalTime)
{
    // Already rendered for this time
    if (currentGlobalTime() == globalTime)
        return true;

    qint64 duration = globalTime.end() - globalTime.start();
    if (nextClipTime().end() == -1) {
        setNextClipTime(QMediaTimeRange::Interval(clipStart(), clipStart() + duration));
    }
    if (active() && clipTimeRange().contains(nextClipTime().start())) {
        if (renderClip(globalTime)) {
            setCurrentGlobalTime(globalTime);
            setNextClipTime(nextClipTime().translated(duration));
            return true;
        } else {
            return false;
        }
    } else if (clipEnd() < nextClipTime().start()) {
        stop();
        return false;
    } else {
        setActive(false);
        return false;
    }
}

void Clip::stop()
{
    setNextClipTime(QMediaTimeRange::Interval(clipStart(), -1));
}