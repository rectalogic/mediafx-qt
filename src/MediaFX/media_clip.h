// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "audio_renderer.h"
#include "decoder.h"
#include "interval.h"
#include <QList>
#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>
#include <QString>
#include <QUrl>
#include <QVideoSink> // IWYU pragma: keep
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
class RenderSession;
using namespace std::chrono;
using namespace std::chrono_literals;

class MediaClip : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged REQUIRED FINAL)
    Q_PROPERTY(int startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged FINAL)
    Q_PROPERTY(int endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged FINAL)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(AudioRenderer* audioRenderer READ audioRenderer WRITE setAudioRenderer NOTIFY audioRendererChanged FINAL)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged FINAL)
    Q_PROPERTY(IntervalGadget currentFrameTime READ currentFrameTime NOTIFY currentFrameTimeChanged FINAL)
    QML_ELEMENT

signals:
    void sourceChanged();
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
    MediaClip(MediaClip&&) = delete;
    MediaClip& operator=(MediaClip&&) = delete;
    ~MediaClip() override;

    QUrl source() const { return m_source; };
    void setSource(const QUrl&);

    qint64 startTime() const { return m_startTime; };
    void setStartTime(qint64 ms);

    qint64 endTime() const { return m_endTime; };
    void setEndTime(qint64 ms);

    qint64 duration() const { return m_endTime - m_startTime; };

    AudioRenderer* audioRenderer() const { return m_audioRenderer; };
    void setAudioRenderer(AudioRenderer* audioRenderer);

    const IntervalGadget currentFrameTime() const { return IntervalGadget(m_currentFrameTime); };

    void setActive(bool active);
    bool isActive() const { return m_active; };

    bool hasAudio() const { return m_decoder && m_decoder->hasAudio(); }
    bool hasVideo() const { return m_decoder && m_decoder->hasVideo(); }

    Q_INVOKABLE void addVideoSink(QVideoSink* videoSink);
    Q_INVOKABLE void removeVideoSink(const QVideoSink* videoSink);

    void render();

    void updateActive();

protected:
    void classBegin() override;
    void componentComplete() override;

    void loadMedia();
    bool isComponentComplete() { return m_componentComplete; };

private slots:
    void onDecoderErrorMessage(const QString& message);

private:
    Q_DISABLE_COPY(MediaClip);

    void setEndTime(const microseconds& us);

    bool m_componentComplete = false;
    bool m_active = false;
    QPointer<RenderSession> m_renderSession;
    QUrl m_source;
    qint64 m_startTime = -1;
    microseconds m_startTimeAdjusted { -1 };
    qint64 m_endTime = -1;
    microseconds m_endTimeAdjusted { -1 };

    int m_frameCount = 1;
    Interval<microseconds> m_currentFrameTime { -1us, -1us };

    std::unique_ptr<Decoder> m_decoder;
    QList<QPointer<QVideoSink>> m_videoSinks;
    QPointer<AudioRenderer> m_audioRenderer;
};
