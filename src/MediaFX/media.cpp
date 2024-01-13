// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

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