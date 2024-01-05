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

#include "media.h"
#include "media_clip.h"
#include "video_track.h"
#include <QList>
#include <QMessageLogContext>
#include <QMetaObject>

MediaAttached* Media::qmlAttachedProperties(QObject* object)
{
    QVideoSink* videoSink = nullptr;
    auto* mo = object->metaObject();
    mo->invokeMethod(object, "videoSink", Q_RETURN_ARG(QVideoSink*, videoSink));
    if (videoSink)
        return new MediaAttached(videoSink, object);
    else {
        qCritical("Media must be attached to an object with a videoSink property");
        return nullptr;
    }
}

void MediaAttached::setClip(MediaClip* clip)
{
    if (clip != m_clip) {
        if (m_clip) {
            VideoTrack* videoTrack = m_clip->videoTrack();
            if (videoTrack) {
                auto sinks = videoTrack->videoSinks();
                if (sinks.removeOne(m_videoSink)) {
                    videoTrack->setVideoSinks(sinks);
                }
            }
        }
        if (clip) {
            VideoTrack* videoTrack = clip->videoTrack();
            if (videoTrack) {
                auto sinks = videoTrack->videoSinks();
                if (!sinks.contains(m_videoSink)) {
                    sinks.append(m_videoSink);
                    videoTrack->setVideoSinks(sinks);
                }
            }
        }
        m_clip = clip;
        emit clipChanged();
    }
}