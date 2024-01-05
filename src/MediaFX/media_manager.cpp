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

#include "media_manager.h"
#include "media_clip.h"
#include <QObject>
#include <chrono>
class QQuickView;
using namespace std::chrono;

MediaManager::MediaManager(const microseconds& frameDuration, QQuickView* quickView, QObject* parent)
    : QObject(parent)
    , m_frameDuration(frameDuration)
    , m_quickView(quickView)
{
    connect(this, &MediaManager::finishEncoding, [this]() { this->finishedEncoding = true; });
}

MediaManager* MediaManager::singletonInstance()
{
    return MediaManagerForeign::s_singletonInstance;
}

void MediaManager::registerClip(MediaClip* clip)
{
    if (clip && !activeClips.contains(clip)) {
        activeClips.append(clip);
    }
}

void MediaManager::unregisterClip(MediaClip* clip)
{
    activeClips.removeOne(clip);
}

void MediaManager::render()
{
    for (auto clip : activeClips) {
        clip->render();
    }
}