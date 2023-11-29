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
#include <QObject>
#include <QQmlInfo>
#include <QUrl>
#include <chrono>
#include <compare>
#include <type_traits>
using namespace std::chrono;
using namespace std::chrono_literals;

Clip::Clip(QObject* parent)
    : QObject(parent)
    , m_clipStart(-1)
    , m_clipEnd(-1)
    , m_currentGlobalTime(-1us, -1us)
    , m_nextClipTime(0us, -1us)
{
    connect(this, &Clip::activeChanged, this, &Clip::onActiveChanged);
}

void Clip::setSource(const QUrl& url)
{
    if (!m_source.isEmpty()) {
        qmlWarning(this) << "Clip source is a write-once property and cannot be changed";
        return;
    }
    m_source = url;
}

void Clip::setClipStart(qint64 ms)
{
    setClipStart_us(milliseconds(ms));
}

void Clip::setClipEnd(qint64 ms)
{
    setClipEnd_us(milliseconds(ms));
}

void Clip::setClipStart_us(microseconds us)
{
    if (m_clipStart != -1us) {
        qmlWarning(this) << "Clip clipStart is a write-once property and cannot be changed";
        return;
    }
    m_clipStart = us;
    emit clipStartChanged();
}

void Clip::setClipEnd_us(microseconds useconds_t)
{
    if (m_clipEnd != -1us) {
        qmlWarning(this) << "Clip clipEnd is a write-once property and cannot be changed";
        return;
    }
    m_clipEnd = useconds_t;
    emit clipEndChanged();
}

bool Clip::render(const Interval& globalTime)
{
    // Already rendered for this time
    if (currentGlobalTime() == globalTime)
        return true;

    if (isActive() && clipSegment().contains(nextClipTime().start())) {
        if (renderClip(globalTime)) {
            setCurrentGlobalTime(globalTime);
            setNextClipTime(nextClipTime().translated(MediaFX::singletonInstance()->session()->frameDuration()));
            return true;
        } else {
            return false;
        }
    } else if (clipEnd_us() < nextClipTime().start()) {
        // XXX add support for looping, just initializeNextClipTime() instead of stop() and set loop on player
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
        clipStart_us(),
        clipStart_us() + MediaFX::singletonInstance()->session()->frameDuration()));
}

void Clip::stop()
{
    initializeNextClipTime();
    setActive(false);
    m_stopped = true;
    emit clipEnded();
}

void Clip::setActive(bool active)
{
    active = active && !m_stopped;
    if (m_active != active) {
        m_active = active;
        auto mediaFX = MediaFX::singletonInstance();
        if (active) {
            mediaFX->registerClip(this);
        } else {
            mediaFX->unregisterClip(this);
        }
        emit onActiveChanged(active);
    }
}

void Clip::classBegin()
{
}

void Clip::componentComplete()
{
    m_componentComplete = true;
    if (clipEnd_us() == -1us) {
        qmlWarning(this) << "Clip has no intrinsic duration, set clipEnd property";
        emit MediaFX::singletonInstance()->session()->exitApp(1);
        return;
    }
    if (clipStart_us() == -1us)
        setClipStart_us(microseconds::zero());

    initializeNextClipTime();
}