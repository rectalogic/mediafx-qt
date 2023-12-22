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
#include "media_clip.h"
#include "session.h"
#include <QObject>

MediaFX::MediaFX(Session* session, QObject* parent)
    : QObject(parent)
    , m_session(session)
{
    connect(this, &MediaFX::finishEncoding, [this](bool immediate) { this->setEncodingState(immediate ? EncodingState::Stopped : EncodingState::Stopping); emit this->session()->exitApp(0); });
}

MediaFX* MediaFX::singletonInstance()
{
    return MediaFXForeign::s_singletonInstance;
}

void MediaFX::registerClip(MediaClip* clip)
{
    if (clip && !activeClips.contains(clip)) {
        activeClips.append(clip);
    }
}

void MediaFX::unregisterClip(MediaClip* clip)
{
    activeClips.removeOne(clip);
}

void MediaFX::render()
{
    for (auto clip : activeClips) {
        clip->render();
    }
}