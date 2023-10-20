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

void Clip::setClipStart(qint64 millis)
{
    setClipStartMicros(millis * 1000);
}

void Clip::setClipEnd(qint64 millis)
{
    setClipEndMicros(millis * 1000);
}

void Clip::setClipStartMicros(qint64 micros)
{
    if (m_clipStart != -1) {
        qmlWarning(this) << "Clip clipStart is a write-once property and cannot be changed";
        return;
    }
    m_clipStart = micros;
    emit clipStartChanged();
}

void Clip::setClipEndMicros(qint64 micros)
{
    if (m_clipEnd != -1) {
        qmlWarning(this) << "Clip clipEnd is a write-once property and cannot be changed";
        return;
    }
    m_clipEnd = micros;
    emit clipEndChanged();
}

bool Clip::render(const QMediaTimeRange::Interval& globalTime)
{
    // Already rendered for this time
    if (currentGlobalTime() == globalTime)
        return true;

    if (active() && clipSegment().contains(nextClipTime().start())) {
        if (renderClip(globalTime)) {
            setCurrentGlobalTime(globalTime);
            setNextClipTime(nextClipTime().translated(MediaFX::singletonInstance()->session()->frameDuration()));
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

void Clip::initializeNextClipTime()
{
    setNextClipTime(QMediaTimeRange::Interval(
        clipStartMicros(),
        MediaFX::singletonInstance()->session()->frameDuration()));
}

void Clip::stop()
{
    initializeNextClipTime();
}

void Clip::setActive(bool active)
{
    auto mediaFX = MediaFX::singletonInstance();
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
    if (clipEndMicros() == -1) {
        qmlWarning(this) << "Clip has no intrinsic duration, set clipEnd property";
        emit MediaFX::singletonInstance()->session()->exitApp(1);
        return;
    }
    if (clipStartMicros() == -1)
        setClipStartMicros(0);

    initializeNextClipTime();
}