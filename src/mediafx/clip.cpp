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

#include "clip.h"
#include "interval.h"
#include "mediafx.h"
#include "session.h"
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

bool Clip::render(const Interval& globalTime)
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
    setNextClipTime(Interval(
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