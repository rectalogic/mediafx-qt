// Copyright (c) 2023 Andrew Wason, All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mediafx.h"
#include "clip.h"
#include <QMediaTimeRange>
#include <QSet>
#include <QmlTypeAndRevisionsRegistration>

int MediaFX::typeId = qmlTypeId("stream.mediafx", 1, 0, "MediaFX");

void MediaFX::registerClip(Clip* clip)
{
    if (clip && !activeClips.contains(clip)) {
        activeClips.append(clip);
        // Ensure we don't have multiple clips simultaneously rendering to the same sink
        QSet<const QVideoSink*> set;
        for (const auto clip : activeClips) {
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

void MediaFX::unregisterClip(Clip* clip)
{
    activeClips.removeOne(clip);
}

// XXX need to signal frame time so QML can react (how will QML normalize for transitions etc.?)
// XXX signaling time will advance QTimeline and may activate/deactivate Clips
bool MediaFX::renderVideoFrame(const QMediaTimeRange::Interval& frameTimeRange)
{
    bool rendered = true;
    for (auto clip : activeClips) {
        rendered = rendered && clip->renderVideoFrame(frameTimeRange);
    }
    return rendered;
}