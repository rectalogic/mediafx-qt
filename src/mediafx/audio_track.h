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

#pragma once

#include "track.h"
#include <QtTypes>
class ErrorInfo;
class Interval;
class MediaClip;
struct FFMS_AudioSource;
struct FFMS_Index;

class AudioTrack : public Track {
public:
    AudioTrack(MediaClip* mediaClip)
        : Track(mediaClip) {};
    ~AudioTrack()
    {
        stop();
    }
    bool initialize(FFMS_Index* index, const char* sourceFile, ErrorInfo& errorInfo) override;
    qint64 duration() const override;
    void stop() override;
    void render(const Interval& frameTime) override;

protected:
    void updateActive();

private:
    FFMS_AudioSource* m_audioSource = nullptr;
};
