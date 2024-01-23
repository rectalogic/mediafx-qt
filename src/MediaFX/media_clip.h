// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interval.h"
#include <QObject>
#include <QQmlParserStatus>
#include <QUrl>
#include <QtQmlIntegration>
#include <QtTypes>
#include <memory>
class AudioRenderer;
class AudioTrack;
class VideoTrack;

class MediaClip : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource REQUIRED FINAL)
    Q_PROPERTY(int startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged FINAL)
    Q_PROPERTY(int endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged FINAL)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(AudioRenderer* audioRenderer READ audioRenderer WRITE setAudioRenderer NOTIFY audioRendererChanged FINAL)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged FINAL)
    Q_PROPERTY(Interval currentFrameTime READ currentFrameTime NOTIFY currentFrameTimeChanged FINAL)
    QML_ELEMENT

signals:
    void startTimeChanged();
    void endTimeChanged();
    void durationChanged();
    void audioRendererChanged();
    void activeChanged();
    void currentFrameTimeChanged();
    void clipEnded();

public:
    using QObject::QObject;

    explicit MediaClip(QObject* parent = nullptr);
    ~MediaClip();

    QUrl source() const { return m_source; };
    void setSource(const QUrl&);

    qint64 startTime() const { return m_startTime; };
    void setStartTime(qint64 ms);

    qint64 endTime() const { return m_endTime; };
    void setEndTime(qint64 ms);

    qint64 duration() const { return m_endTime - m_startTime; };

    AudioRenderer* audioRenderer() const { return m_audioRenderer; };
    void setAudioRenderer(AudioRenderer* audioRenderer);

    const Interval& currentFrameTime() const { return m_currentFrameTime; };

    void setActive(bool active);
    bool isActive() const { return m_active; };

    void render();

    void classBegin() override {};
    void componentComplete() override;

    void updateActive();

protected:
    void loadMedia();

    bool isComponentComplete() { return m_componentComplete; };

    VideoTrack* videoTrack() { return m_videoTrack.get(); };
    AudioTrack* audioTrack() { return m_audioTrack.get(); };

    friend class MediaManager;

private:
    bool m_componentComplete = false;
    bool m_active = false;
    QUrl m_source;
    qint64 m_startTime;
    qint64 m_endTime;

    Interval m_currentFrameTime;

    std::unique_ptr<VideoTrack> m_videoTrack;
    std::unique_ptr<AudioTrack> m_audioTrack;
    AudioRenderer* m_audioRenderer;
};
