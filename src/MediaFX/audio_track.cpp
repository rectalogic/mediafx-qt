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

#include "audio_track.h"
#include "error_info.h"
#include <ffms.h>
#include <math.h>

bool AudioTrack::initialize(FFMS_Index* index, const char* sourceFile, ErrorInfo& errorInfo)
{
    int audioTrackNum = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_AUDIO, &errorInfo);
    if (audioTrackNum >= 0) {
        FFMS_AudioSource* audioSource = FFMS_CreateAudioSource(sourceFile, audioTrackNum, index, FFMS_DELAY_FIRST_VIDEO_TRACK, &errorInfo);
        if (audioSource) {
            m_audioSource = audioSource;
            updateActive();
            return true;
        }
    }
    return false;
}

qint64 AudioTrack::duration() const
{
    if (!m_audioSource)
        return 0;
    const FFMS_AudioProperties* audioProps = FFMS_GetAudioProperties(m_audioSource);
    return round(audioProps->LastEndTime * 1000);
}

void AudioTrack::updateActive()
{
    setActive(m_audioSource);
}

void AudioTrack::render(const Interval& frameTime)
{
}

void AudioTrack::stop()
{
    if (m_audioSource)
        FFMS_DestroyAudioSource(m_audioSource);
    m_audioSource = nullptr;
    updateActive();
}