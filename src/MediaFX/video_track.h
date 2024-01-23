// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "track.h"
#include <QList>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QtTypes>
#include <ffms.h>
#include <memory>
#include <utility>
class ErrorInfo;
class Interval;
class MediaClip;
class QVideoSink;

class VideoTrack : public Track {
public:
    VideoTrack(MediaClip* mediaClip);
    ~VideoTrack();

    bool initialize(FFMS_Index* index, int trackNum, const char* sourceFile, ErrorInfo& errorInfo) override;
    qint64 duration() const override;
    void render(const Interval& frameTime);
    void stop() override;

    void addVideoSink(QVideoSink* videoSink);
    void removeVideoSink(const QVideoSink* videoSink);

protected:
    const QList<QVideoSink*>& videoSinks() const { return m_videoSinks; };
    void updateActive();

private:
    inline qint64 calculateFrameStartTime(const FFMS_FrameInfo* frameInfo) const;
    QVideoFrameFormat formatForFrame(const FFMS_Frame* frameProperties) const;
    void mapVideoFrameData(QVideoFrame& videoFrame, const FFMS_Frame* frameProperties);

    struct VideoSourceDeleter {
        void operator()(FFMS_VideoSource* s) const
        {
            FFMS_DestroyVideoSource(s);
        }
    };

    typedef std::unique_ptr<FFMS_VideoSource, VideoSourceDeleter> VideoSourcePtr;
    VideoSourcePtr m_videoSourcePtr;
    FFMS_Track* m_track = nullptr;
    int m_nextFrameNum = 0;
    const FFMS_FrameInfo* m_nextFrameInfo = nullptr;
    const FFMS_TrackTimeBase* m_timebase = nullptr;
    QList<QVideoSink*> m_videoSinks;
    std::pair<QVideoFrame, QVideoFrame> m_videoFrame;
};
