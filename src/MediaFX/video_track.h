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
    const QList<QVideoSink*>& videoSinks() const { return m_videoSinks; };

protected:
    friend class MediaAttached;
    void setVideoSinks(const QList<QVideoSink*>&);
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
