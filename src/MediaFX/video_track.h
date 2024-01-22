// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "track.h"
#include <QList>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QtTypes>
#include <utility>
class ErrorInfo;
class Interval;
class MediaClip;
class QVideoSink;
struct FFMS_Frame;
struct FFMS_FrameInfo;
struct FFMS_Index;
struct FFMS_Track;
struct FFMS_TrackTimeBase;
struct FFMS_VideoSource;

class VideoTrack : public Track {
public:
    VideoTrack(MediaClip* mediaClip)
        : Track(mediaClip)
        , m_videoFrame(QVideoFrame(), QVideoFrame())
    {
    }
    ~VideoTrack()
    {
        stop();
    }

    bool initialize(FFMS_Index* index, const char* sourceFile, ErrorInfo& errorInfo) override;
    qint64 duration() const override;
    void render(const Interval& frameTime) override;
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

    FFMS_VideoSource* m_videoSource = nullptr;
    FFMS_Track* m_track = nullptr;
    int m_nextFrameNum = 0;
    const FFMS_FrameInfo* m_nextFrameInfo = nullptr;
    const FFMS_TrackTimeBase* m_timebase = nullptr;
    QList<QVideoSink*> m_videoSinks;
    std::pair<QVideoFrame, QVideoFrame> m_videoFrame;
};
