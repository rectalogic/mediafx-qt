// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interval.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QJSEngine>
#include <QList>
#include <QObject>
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
extern "C" {
#include <libavutil/rational.h>
}
Q_MOC_INCLUDE("audio_renderer.h")
Q_MOC_INCLUDE("media_clip.h")
Q_MOC_INCLUDE(<QQuickView>)
Q_MOC_INCLUDE(<QVideoSink>)
class AudioRenderer;
class MediaClip;
class OutputFormat;
class QQuickView;
class QQmlEngine;
class QVideoSink;
using namespace std::chrono;

class MediaManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickView* window READ window CONSTANT FINAL)
    Q_PROPERTY(IntervalGadget currentRenderTime READ currentRenderTime NOTIFY currentRenderTimeChanged FINAL)

public:
    using QObject::QObject;
    MediaManager(const OutputFormat& outputFormat, QQuickView* quickView, QObject* parent = nullptr);
    MediaManager(MediaManager&&) = delete;
    MediaManager& operator=(MediaManager&&) = delete;
    ~MediaManager() override;

    static MediaManager* singletonInstance();

    Q_INVOKABLE IntervalGadget createInterval(qint64 start, qint64 end) const
    {
        return IntervalGadget(start, end);
    };

    Q_INVOKABLE void updateVideoSinks(MediaClip* oldClip, MediaClip* newClip, QVideoSink* videoSink);

    QQuickView* window() const { return m_quickView; };
    const AVRational& outputFrameRate() const { return m_outputFrameRate; }
    const QAudioFormat& outputAudioFormat() const { return m_outputAudioFormat; }
    QAudioBuffer createOutputAudioBuffer();
    const IntervalGadget currentRenderTime() const { return IntervalGadget(m_currentRenderTime); };
    void nextRenderTime();

    AudioRenderer* audioRenderer() const { return m_rootAudioRenderer.get(); };

    void registerClip(MediaClip* clip);
    void unregisterClip(MediaClip* clip);

    void render();

    Q_INVOKABLE void pauseRendering();
    Q_INVOKABLE void resumeRendering();
    bool isRenderingPaused() const { return m_pauseRendering > 0; }
    bool isFinishedEncoding() const { return finishedEncoding; }

signals:
    void currentRenderTimeChanged();
    void renderingPausedChanged();

public slots:
    void finishEncoding();
    void fatalError() const;

private:
    Q_DISABLE_COPY(MediaManager);

    AVRational m_outputFrameRate;
    QAudioFormat m_outputAudioFormat;
    Interval<microseconds> m_currentRenderTime;
    int m_frameCount = 1;
    QQuickView* m_quickView;
    std::unique_ptr<AudioRenderer> m_rootAudioRenderer;
    QList<MediaClip*> activeClips;
    bool finishedEncoding = false;
    int m_pauseRendering = 0;
};

// https://doc.qt.io/qt-6/qqmlengine.html#QML_SINGLETON
struct MediaManagerForeign {
    Q_GADGET
    QML_FOREIGN(MediaManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(MediaManager)
public:
    static void setSingletonInstance(MediaManager* manager)
    {
        Q_ASSERT(!s_singletonInstance);
        s_singletonInstance = manager;
    }

    static MediaManager* singletonInstance()
    {
        return s_singletonInstance;
    }

    static MediaManager* create(QQmlEngine*, QJSEngine* engine)
    {
        // The instance has to exist before it is used. We cannot replace it.
        Q_ASSERT(s_singletonInstance);

        // The engine has to have the same thread affinity as the singleton.
        Q_ASSERT(engine->thread() == s_singletonInstance->thread());

        // There can only be one engine accessing the singleton.
        if (s_engine)
            Q_ASSERT(engine == s_engine);
        else
            s_engine = engine;

        // Explicitly specify C++ ownership so that the engine doesn't delete
        // the instance.
        QJSEngine::setObjectOwnership(s_singletonInstance, QJSEngine::CppOwnership);
        return s_singletonInstance;
    }

private:
    inline static MediaManager* s_singletonInstance = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    inline static QJSEngine* s_engine = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
};