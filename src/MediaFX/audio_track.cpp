// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

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