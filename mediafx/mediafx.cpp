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