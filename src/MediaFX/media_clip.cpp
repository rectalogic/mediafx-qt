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
#include <QQmlEngine>
#include <QQmlInfo>
#include <QQuickView>
#include <QString>
#include <QUrl>
#include <algorithm>
#include <chrono>
#include <ffms.h>

MediaClip::MediaClip(QObject* parent)
    : QObject(parent)
    , m_startTime(-1)
    , m_endTime(-1)
    , m_currentFrameTime(-1, -1)
    , m_videoTrack(new VideoTrack(this))
    , m_audioTrack(new AudioTrack(this))
{
}

MediaClip::~MediaClip() = default;

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
}

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

void MediaClip::render()
{
    if (!isActive())
        return;

    emit currentFrameTimeChanged();

    if (m_audioTrack)
        m_audioTrack->render(m_currentFrameTime);
    if (m_videoTrack)
        m_videoTrack->render(m_currentFrameTime);

    m_currentFrameTime = m_currentFrameTime.nextInterval(MediaManager::singletonInstance()->frameDuration());

    if (m_currentFrameTime.start() >= m_endTime) {
        if (m_audioTrack)
            m_audioTrack->stop();
        if (m_videoTrack)
            m_videoTrack->stop();
        emit clipEnded();
        return;
    }
}

void MediaClip::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        auto mediaFX = MediaManager::singletonInstance();
        if (active) {
            mediaFX->registerClip(this);
        } else {
            mediaFX->unregisterClip(this);
        }
    }
}

void MediaClip::updateActive()
{
    setActive((m_videoTrack && m_videoTrack->isActive()) || (m_audioTrack && m_audioTrack->isActive()));
}

void MediaClip::loadMedia()
{
    if (!source().isValid()) {
        qmlWarning(this) << "MediaClip requires source Url";
        emit MediaManager::singletonInstance()->window()->engine()->exit(1);
        return;
    }
    ErrorInfo errorInfo;
    QString sourceFile = source().toLocalFile();
    QByteArray sourceFileUtf8 = sourceFile.toUtf8();
    FFMS_Indexer* indexer = FFMS_CreateIndexer(sourceFileUtf8.data(), &errorInfo);
    if (!indexer) {
        qmlWarning(this) << "MediaClip FFMS_CreateIndexer failed:" << errorInfo;
        emit MediaManager::singletonInstance()->window()->engine()->exit(1);
        return;
    }

    FFMS_Index* index = FFMS_DoIndexing2(indexer, FFMS_IEH_ABORT, &errorInfo);
    if (!index) {
        qmlWarning(this) << "MediaClip FFMS_DoIndexing2 failed:" << errorInfo;
        emit MediaManager::singletonInstance()->window()->engine()->exit(1);
        return;
    }

    if (!m_videoTrack->initialize(index, sourceFileUtf8.data(), errorInfo)) {
        m_videoTrack.reset(nullptr);
        qmlWarning(this) << "VideoTrack initialize failed:" << errorInfo;
        errorInfo.reset();
    }

    if (m_audioTrack->initialize(index, sourceFileUtf8.data(), errorInfo)) {
        m_audioTrack.reset(nullptr);
        qmlWarning(this) << "AudioTrack initialize failed:" << errorInfo;
        errorInfo.reset();
    }

    FFMS_DestroyIndex(index);
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
    m_currentFrameTime = Interval(milliseconds(startTime()), milliseconds(startTime()) + MediaManager::singletonInstance()->frameDuration());
}