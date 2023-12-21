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

#include "video_track.h"
#include "error_info.h"
#include "interval.h"
#include "media_clip.h"
#include <QDebug>
#include <QList>
#include <QQmlInfo>
#include <QSize>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QVideoSink>
#include <ffms.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

bool VideoTrack::initialize(FFMS_Index* index, const char* sourceFile, ErrorInfo& errorInfo)
{
    int videoTrackNum = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_VIDEO, &errorInfo);
    if (videoTrackNum >= 0) {
        FFMS_VideoSource* videoSource = FFMS_CreateVideoSource(sourceFile, videoTrackNum, index, 1, FFMS_SEEK_LINEAR, &errorInfo);
        if (videoSource) {
            const FFMS_Frame* frameProperties = FFMS_GetFrame(videoSource, 0, &errorInfo);
            if (frameProperties) {
                int pixelFormats[2] = { FFMS_GetPixFmt("rgba"), -1 };
                if (FFMS_SetOutputFormatV2(videoSource, pixelFormats, frameProperties->EncodedWidth, frameProperties->EncodedHeight, FFMS_RESIZER_BICUBIC, &errorInfo) == 0) {
                    m_videoSource = videoSource;
                    m_track = FFMS_GetTrackFromVideo(m_videoSource);
                    m_nextFrameInfo = FFMS_GetFrameInfo(m_track, m_nextFrameNum);
                    m_timebase = FFMS_GetTimeBase(m_track);
                    QVideoFrameFormat format = QVideoFrameFormat(QSize(frameProperties->EncodedWidth, frameProperties->EncodedHeight), QVideoFrameFormat::Format_RGBA8888);
                    m_videoFrame.first = QVideoFrame(format);
                    m_videoFrame.second = QVideoFrame(format);
                    // Prime first frames data
                    if (mediaClip()->clipStart() == 0) {
                        mapVideoFrameData(frameProperties);
                    }
                    updateActive();
                    return true;
                }
            }
        }
    }
    return false;
}

void VideoTrack::mapVideoFrameData(const FFMS_Frame* frameProperties)
{
    m_videoFrame.first.map(QVideoFrame::WriteOnly);
    memcpy(m_videoFrame.first.bits(0), frameProperties->Data[0], m_videoFrame.first.mappedBytes(0));
    m_videoFrame.first.unmap();
}

qint64 VideoTrack::calculateFrameStartTime(const FFMS_FrameInfo* frameInfo) const
{
    return (qint64)((frameInfo->PTS * m_timebase->Num) / (double)m_timebase->Den);
}

qint64 VideoTrack::duration() const
{
    if (!m_videoSource)
        return 0;
    const FFMS_VideoProperties* videoProps = FFMS_GetVideoProperties(m_videoSource);
    return round(videoProps->LastEndTime * 1000);
}

void VideoTrack::setVideoSinks(const QList<QVideoSink*>& videoSinks)
{
    if (m_videoSinks != videoSinks) {
        m_videoSinks = videoSinks;
        updateActive();
    }
}

void VideoTrack::updateActive()
{
    setActive(!videoSinks().isEmpty() && m_videoSource);
}

void VideoTrack::render(const Interval& frameTime)
{
    auto frameInfo = m_nextFrameInfo;
    while (frameInfo && calculateFrameStartTime(frameInfo) < frameTime.start()) {
        m_nextFrameNum++;
        frameInfo = FFMS_GetFrameInfo(m_track, m_nextFrameNum);
    }
    // EOF if no frame info, continue sending last frame rendered
    if (frameInfo && frameInfo != m_nextFrameInfo) {
        m_nextFrameInfo = frameInfo;
        if (isActive()) {
            ErrorInfo errorInfo;
            const FFMS_Frame* frameProperties = FFMS_GetFrame(m_videoSource, m_nextFrameNum, &errorInfo);
            if (!frameProperties) {
                qmlWarning(mediaClip()) << "FFMS_GetFrame failed on frame" << m_nextFrameNum << errorInfo;
                stop();
                return;
            }
            // Swap frames because VideoOutput just compares internal frame pointers and ignores if the same
            std::swap(m_videoFrame.first, m_videoFrame.second);
            mapVideoFrameData(frameProperties);
        }
    }

    // We consume frames above if inactive since audio may be active. Only send frames if active.
    if (isActive()) {
        for (auto videoSink : videoSinks()) {
            videoSink->setVideoFrame(m_videoFrame.first);
        }
    }
}

void VideoTrack::stop()
{
    if (m_videoSource)
        FFMS_DestroyVideoSource(m_videoSource);
    m_videoSource = nullptr;
    m_track = nullptr;
    m_timebase = nullptr;
    m_videoFrame.first = QVideoFrame();
    m_videoFrame.second = QVideoFrame();
    updateActive();
}
