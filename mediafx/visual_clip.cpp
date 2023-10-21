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