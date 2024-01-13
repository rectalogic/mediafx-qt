// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

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