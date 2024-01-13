// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

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
