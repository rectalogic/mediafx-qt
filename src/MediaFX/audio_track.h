// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "track.h"
#include <QAudioBuffer>
#include <QtTypes>
#include <ffms.h>
#include <memory>
class AudioRenderer;
class ErrorInfo;
class Interval;
class MediaClip;
class Resampler;

class AudioTrack : public Track {
public:
    AudioTrack(MediaClip* mediaClip);
    ~AudioTrack();
    bool initialize(FFMS_Index* index, int trackNum, const char* sourceFile, ErrorInfo& errorInfo) override;
    qint64 duration() const override;
    void stop() override;
    void render(const Interval& frameTime, AudioRenderer* audioRenderer);

protected:
    void updateActive();

private:
    struct AudioSourceDeleter {
        void operator()(FFMS_AudioSource* s) const
        {
            FFMS_DestroyAudioSource(s);
        }
    };

    typedef std::unique_ptr<FFMS_AudioSource, AudioSourceDeleter> AudioSourcePtr;
    AudioSourcePtr m_audioSourcePtr;
    int m_nextFrameNum = 0;
    QAudioBuffer m_outputAudioBuffer;
    std::unique_ptr<Resampler> m_resamplerPtr;
};
