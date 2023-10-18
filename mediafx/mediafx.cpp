// Copyright (c) 2023 Andrew Wason, All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mediafx.h"
#include "visual_clip.h"
#include <QDebug>
#include <QMediaTimeRange>
#include <QMessageLogContext>
#include <QObject>
#include <QSet>
#include <QmlTypeAndRevisionsRegistration>
class QVideoSink;

// Can't use this, so we defer setting typeId
// https://bugreports.qt.io/browse/QTBUG-118165
// int MediaFX::typeId = qmlTypeId("stream.mediafx", 254, 254, "MediaFX");
int MediaFX::typeId = -1;

MediaFX::MediaFX()
    : QObject()
{
    MediaFX::typeId = qmlTypeId("stream.mediafx", 254, 254, "MediaFX");
}

void MediaFX::registerVisualClip(VisualClip* clip)
{
    if (clip && !activeVisualClips.contains(clip)) {
        activeVisualClips.append(clip);
        // Ensure we don't have multiple clips simultaneously rendering to the same sink
        QSet<const QVideoSink*> set;
        for (const auto clip : activeVisualClips) {
            for (const auto sink : clip->videoSinks()) {
                if (set.contains(sink)) {
                    qWarning() << "Warning: duplicate QVideoSink found on " << clip;
                    return;
                }
                set.insert(sink);
            }
        }
    }
}

void MediaFX::unregisterVisualClip(VisualClip* clip)
{
    activeVisualClips.removeOne(clip);
}

// XXX need to signal frame time so QML can react (how will QML normalize for transitions etc.?)
// XXX signaling time will advance QTimeline and may activate/deactivate Clips
bool MediaFX::renderVideoFrame(const QMediaTimeRange::Interval& frameTimeRange)
{
    bool rendered = true;
    for (auto clip : activeVisualClips) {
        rendered = rendered && clip->renderVideoFrame(frameTimeRange);
    }
    return rendered;
}