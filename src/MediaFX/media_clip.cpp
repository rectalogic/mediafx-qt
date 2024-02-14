// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media_clip.h"
#include "audio_track.h"
#include "error_info.h"
#include "interval.h"
#include "media_manager.h"
#include "video_track.h"
#include <QByteArray>
#include <QDebug>
#include <QObject>
#include <QQmlInfo>
#include <QString>
#include <QUrl>
#include <algorithm>
#include <chrono>
#include <ffms.h>
class AudioRenderer;

/*!
    \qmltype MediaClip
    //! \instantiates MediaClip
    \inqmlmodule MediaFX

    \brief Plays audio and video frames from a \l source.
*/
MediaClip::MediaClip(QObject* parent)
    : QObject(parent)
    , m_startTime(-1)
    , m_endTime(-1)
    , m_currentFrameTime(-1, -1)
    , m_audioRenderer(nullptr)
    , m_videoTrack(new VideoTrack(this))
    , m_audioTrack(new AudioTrack(this))
{
}

MediaClip::~MediaClip() = default;

/*!
    \qmlproperty url MediaClip::source

    The source media file url.
*/
void MediaClip::setSource(const QUrl& url)
{
    if (url.isEmpty()) {
        qmlWarning(this) << "MediaClip source is a write-once property and cannot be changed";
        return;
    }
    if (!url.isLocalFile()) {
        qmlWarning(this) << "MediaClip source requires a local file Url";
        return;
    }
    m_source = url;
    emit sourceChanged();
}

/*!
    \qmlproperty int MediaClip::startTime

    The start time offset (in milliseconds) of the clip.
    Defaults to 0.
*/
void MediaClip::setStartTime(qint64 ms)
{
    if (m_startTime != -1) {
        qmlWarning(this) << "MediaClip startTime is a write-once property and cannot be changed";
        return;
    }
    m_startTime = ms;
    emit startTimeChanged();
    emit durationChanged();
}

/*!
    \qmlproperty int MediaClip::endTime

    The end time (in milliseconds) of the clip.
    Defaults to the clips duration.
*/
void MediaClip::setEndTime(qint64 ms)
{
    if (m_endTime != -1) {
        qmlWarning(this) << "MediaClip endTime is a write-once property and cannot be changed";
        return;
    }
    m_endTime = ms;
    emit endTimeChanged();
    emit durationChanged();
}

/*!
    \qmlproperty int MediaClip::duration

    The duration (in milliseconds) of the clip.
    This is (\l endTime - \l startTime).
*/

/*!
    \qmlproperty int MediaClip::audioRenderer

    The \l AudioRenderer to render audio into.
*/
void MediaClip::setAudioRenderer(AudioRenderer* audioRenderer)
{
    if (audioRenderer != m_audioRenderer) {
        m_audioRenderer = audioRenderer;
        emit audioRendererChanged();
    }
}

void MediaClip::render()
{
    if (!isActive())
        return;

    emit currentFrameTimeChanged();

    if (m_audioTrack)
        m_audioTrack->render(m_currentFrameTime, m_audioRenderer);
    if (m_videoTrack)
        m_videoTrack->render(m_currentFrameTime);

    m_currentFrameTime = m_currentFrameTime.nextInterval(MediaManager::singletonInstance()->outputVideoFrameDuration());

    if (m_currentFrameTime.start() >= m_endTime) {
        if (m_audioTrack)
            m_audioTrack->stop();
        if (m_videoTrack)
            m_videoTrack->stop();
        emit clipEnded();
        return;
    }
}

/*!
    \qmlproperty bool MediaClip::active

    \c true if the clip is currently rendering video frames into a \l VideoRenderer.
*/
void MediaClip::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        auto manager = MediaManager::singletonInstance();
        if (active) {
            manager->registerClip(this);
        } else {
            manager->unregisterClip(this);
        }
        emit activeChanged();
    }
}

void MediaClip::updateActive()
{
    // We are active if we are rendering video, or we have no video track and are rendering audio
    setActive((m_videoTrack && m_videoTrack->isActive()) || (!m_videoTrack && m_audioTrack && m_audioTrack->isActive()));
}

void MediaClip::loadMedia()
{
    if (!source().isValid()) {
        qmlWarning(this) << "MediaClip requires source Url";
        MediaManager::singletonInstance()->fatalError();
        return;
    }
    ErrorInfo errorInfo;
    QString sourceFile = source().toLocalFile();
    QByteArray sourceFileUtf8 = sourceFile.toUtf8();
    FFMS_Indexer* indexer = FFMS_CreateIndexer(sourceFileUtf8.data(), &errorInfo);
    if (!indexer) {
        qmlWarning(this) << "MediaClip FFMS_CreateIndexer failed:" << errorInfo;
        MediaManager::singletonInstance()->fatalError();
        return;
    }
    FFMS_TrackTypeIndexSettings(indexer, FFMS_TYPE_VIDEO, 1, 0);
    FFMS_TrackTypeIndexSettings(indexer, FFMS_TYPE_AUDIO, 1, 0);

    std::unique_ptr<FFMS_Index, decltype(&FFMS_DestroyIndex)> index(FFMS_DoIndexing2(indexer, FFMS_IEH_ABORT, &errorInfo), FFMS_DestroyIndex);
    if (!index) {
        qmlWarning(this) << "MediaClip FFMS_DoIndexing2 failed:" << errorInfo;
        MediaManager::singletonInstance()->fatalError();
        return;
    }

    int videoTrackNum = FFMS_GetFirstTrackOfType(index.get(), FFMS_TYPE_VIDEO, &errorInfo);
    if (videoTrackNum >= 0) {
        if (!m_videoTrack->initialize(index.get(), videoTrackNum, sourceFileUtf8.data(), errorInfo)) {
            m_videoTrack.reset(nullptr);
            qmlWarning(this) << "VideoTrack initialize failed:" << errorInfo;
            errorInfo.reset();
        }
    } else {
        m_videoTrack.reset(nullptr);
    }

    int audioTrackNum = FFMS_GetFirstTrackOfType(index.get(), FFMS_TYPE_AUDIO, &errorInfo);
    if (audioTrackNum >= 0) {
        if (!m_audioTrack->initialize(index.get(), audioTrackNum, sourceFileUtf8.data(), errorInfo)) {
            m_audioTrack.reset(nullptr);
            qmlWarning(this) << "AudioTrack initialize failed:" << errorInfo;
            errorInfo.reset();
        }
    } else {
        m_audioTrack.reset(nullptr);
    }

    if (videoTrackNum < 0 && audioTrackNum < 0) {
        qmlWarning(this) << "No audio or video tracks found";
    }
    updateActive();
}

void MediaClip::componentComplete()
{
    m_componentComplete = true;
    if (startTime() < 0)
        setStartTime(0);
    loadMedia();
    if (endTime() < 0) {
        setEndTime(std::max(m_audioTrack ? m_audioTrack->duration() : 0, m_videoTrack ? m_videoTrack->duration() : 0));
    }
    m_currentFrameTime = Interval(milliseconds(startTime()), milliseconds(startTime()) + MediaManager::singletonInstance()->outputVideoFrameDuration());
}

/*!
    \qmlproperty interval MediaClip::currentFrameTime

    The time \l interval of the currently rendered video frame.
*/

/*!
    \qmlsignal MediaClip::clipEnded()

    This signal is emitted when the clip has finished playback.
*/