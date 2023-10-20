// Copyright (c) 2023 Andrew Wason, All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mediafx.h"
#include "clip.h"
#include <QMediaTimeRange>

MediaFX* MediaFX::singletonInstance()
{
    return MediaFXForeign::s_singletonInstance;
}

void MediaFX::registerClip(Clip* clip)
{
    if (clip && !activeClips.contains(clip)) {
        activeClips.append(clip);
#if 0 // XXX how should we deal with this? same issue for main audio, don't want to mix audio buffers from different Clips
//XXX move this into Clip subclasses setActive, so they can query other activeClips and check?
      // Ensure we don't have multiple clips simultaneously rendering to the same sink
        QSet<const QVideoSink*> set;
        for (const auto clip : activeClips) {
            for (const auto sink : clip->videoSinks()) {
                if (set.contains(sink)) {
                    qmlWarning(clip) << "Warning: duplicate QVideoSink found";
                    return;
                }
                set.insert(sink);
            }
        }
#endif
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
        rendered = rendered && clip->render(frameTimeRange);
    }
    return rendered;
}