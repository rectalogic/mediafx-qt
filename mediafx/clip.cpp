// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include "mediafx.h"
#include "session.h"
#include <QMediaTimeRange>
#include <QQmlEngine>
#include <QQmlInfo>
#include <QUrl>
#include <QmlTypeAndRevisionsRegistration>

void Clip::setSource(const QUrl& url)
{
    if (!m_source.isEmpty()) {
        qmlWarning(this) << "Clip source is a write-once property and cannot be changed";
        return;
    }
    m_source = url;
}

void Clip::setClipStart(qint64 clipStart)
{
    if (m_clipStart != -1) {
        qmlWarning(this) << "Clip clipStart is a write-once property and cannot be changed";
        return;
    }
    m_clipStart = clipStart;
    emit clipStartChanged();
}

void Clip::setClipEnd(qint64 clipEnd)
{
    if (m_clipEnd != -1) {
        qmlWarning(this) << "Clip clipEnd is a write-once property and cannot be changed";
        return;
    }
    m_clipEnd = clipEnd;
    emit clipEndChanged();
}

bool Clip::render(const QMediaTimeRange::Interval& globalTime)
{
    // Already rendered for this time
    if (currentGlobalTime() == globalTime)
        return true;

    qint64 frameDuration = globalTime.end() - globalTime.start();
    if (active() && clipSegment().contains(nextClipTime().start())) {
        if (renderClip(globalTime)) {
            setCurrentGlobalTime(globalTime);
            setNextClipTime(nextClipTime().translated(frameDuration));
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
    setNextClipTime(QMediaTimeRange::Interval(
        clipStart(),
        qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId)->session()->frameDuration()));
}

void Clip::setActive(bool active)
{
    auto mediaFX = qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId);
    if (active) {
        mediaFX->registerClip(this);
    } else {
        mediaFX->unregisterClip(this);
    }
}

void Clip::classBegin()
{
}

void Clip::componentComplete()
{
    m_componentComplete = true;
    if (clipEnd() == -1) {
        qmlWarning(this) << "Clip has no intrinsic duration, set clipEnd property";
        emit qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId)->session()->exitApp(1);
        return;
    }
    if (clipStart() == -1)
        setClipStart(0);

    setNextClipTime(QMediaTimeRange::Interval(
        clipStart(),
        qmlEngine(this)->singletonInstance<MediaFX*>(MediaFX::typeId)->session()->frameDuration()));
}