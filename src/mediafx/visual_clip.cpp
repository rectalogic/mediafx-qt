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
#include <QList>
#include <QMessageLogContext>
#include <QMetaObject>
#include <QVideoSink>
struct Interval;

void VisualClip::setVideoSinks(const QList<QVideoSink*>& videoSinks)
{
    if (m_videoSinks != videoSinks) {
        m_videoSinks = videoSinks;
        setActive(!videoSinks.isEmpty());
    }
}

bool VisualClip::renderClip(const Interval& globalTime)
{
    if (!prepareNextVideoFrame(globalTime))
        return false;
    for (auto videoSink : videoSinks()) {
        videoSink->setVideoFrame(m_currentVideoFrame);
    }
    return true;
}

VisualSinkAttached* VisualSink::qmlAttachedProperties(QObject* object)
{
    QVideoSink* videoSink = nullptr;
    auto* mo = object->metaObject();
    mo->invokeMethod(object, "videoSink", Q_RETURN_ARG(QVideoSink*, videoSink));
    if (videoSink)
        return new VisualSinkAttached(videoSink, object);
    else {
        qCritical("VisualSink must be attached to an object with a videoSink property");
        return nullptr;
    }
}

void VisualSinkAttached::setClip(VisualClip* clip)
{
    if (clip != m_clip) {
        if (m_clip) {
            auto sinks = m_clip->videoSinks();
            if (sinks.removeOne(m_videoSink)) {
                m_clip->setVideoSinks(sinks);
            }
        }
        if (clip) {
            auto sinks = clip->videoSinks();
            if (!sinks.contains(m_videoSink)) {
                sinks.append(m_videoSink);
                clip->setVideoSinks(sinks);
            }
        }
        emit clipChanged();
    }
}