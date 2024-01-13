// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "track.h"
#include "media_clip.h"

void Track::setActive(bool active)
{
    if (active != m_active) {
        m_active = active;
        m_mediaClip->updateActive();
    }
}