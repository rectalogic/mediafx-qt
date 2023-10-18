// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include <QDebug>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QUrl>

void Clip::setSource(const QUrl& url)
{
    if (!m_source.isEmpty()) {
        qWarning() << "Clip source is a write-once property and cannot be changed";
        return;
    }
    m_source = url;
}

void Clip::setClipStart(qint64 clipStart)
{
    if (m_clipStart != -1) {
        qWarning() << "Clip clipStart is a write-once property and cannot be changed";
        return;
    }
    m_clipStart = clipStart;
}

void Clip::setClipEnd(qint64 clipEnd)
{
    if (m_clipEnd != -1) {
        qWarning() << "Clip clipEnd is a write-once property and cannot be changed";
        return;
    }
    m_clipEnd = clipEnd;
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
    if (active() && clipSegment().contains(nextClipTime().start())) {
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

void Clip::classBegin()
{
}

void Clip::componentComplete()
{
    m_componentComplete = true;
    // Clip can return clipEnd() if no intrinsic duration, ensure it is set
    if (duration() == -1) {
        qCritical() << "Clip has no intrinsic duration, set clipEnd property";
        QCoreApplication::exit();
        return;
    }
    if (clipStart() == -1)
        setClipStart(0);
    if (clipEnd() == -1) {
        setClipEnd(duration() - clipStart());
    }
    // We don't know frame duration yet, it will be initialized on first render, set to -1
    setNextClipTime(QMediaTimeRange::Interval(clipStart(), -1));
}