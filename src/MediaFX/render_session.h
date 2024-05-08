// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interval.h"
#include "render_context.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QRectF>
#include <QUrl>
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
class AnimationDriver;
class AudioRenderer;
class RenderSessionAttached;
using namespace std::chrono;

class RenderSession : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl WRITE setSourceUrl NOTIFY sourceUrlChanged FINAL REQUIRED)
    Q_PROPERTY(IntervalGadget currentRenderTime READ currentRenderTime NOTIFY currentRenderTimeChanged FINAL)
    Q_PROPERTY(Rational frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged FINAL)
    Q_PROPERTY(int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged FINAL)
    QML_ATTACHED(RenderSessionAttached)
    QML_ELEMENT

public:
    using QQuickItem::QQuickItem;

    RenderSession(QQuickItem* parent = nullptr);
    RenderSession(RenderSession&&) = delete;
    RenderSession& operator=(RenderSession&&) = delete;
    ~RenderSession() override;

    static RenderSessionAttached* qmlAttachedProperties(QObject* object);

    Q_INVOKABLE IntervalGadget createInterval(qint64 start, qint64 end) const
    {
        return IntervalGadget(start, end);
    };

    const QUrl& sourceUrl() const { return m_sourceUrl; }
    void setSourceUrl(const QUrl& url);

    const Rational& frameRate() const { return m_frameRate; }
    void setFrameRate(const Rational& frameRate);

    int sampleRate() const { return m_sampleRate; }
    void setSampleRate(int sampleRate);

    const QAudioFormat& outputAudioFormat() const { return m_outputAudioFormat; }
    const IntervalGadget currentRenderTime() const { return IntervalGadget(m_currentRenderTime); }

    AudioRenderer* rootAudioRenderer() const { return m_rootAudioRenderer.get(); }
    const QAudioBuffer& silentOutputAudioBuffer();

    void render();
    bool event(QEvent* event) override;

    Q_INVOKABLE void pauseRendering();
    Q_INVOKABLE void resumeRendering();
    bool isRenderingPaused() const { return m_pauseRendering > 0; }
    bool isSessionEnded() const { return m_sessionEnded; }

    static RenderSession* findSession(QObject* object);

signals:
    void sourceUrlChanged();
    void frameRateChanged();
    void sampleRateChanged();
    void currentRenderTimeChanged();
    void sessionEnded();
    void renderMediaClips();
    void renderScene();

public slots:
    void beginSession();
    void endSession();
    void fatalError() const;

protected:
    void postRenderEvent();
    void classBegin() override { }
    void componentComplete() override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    Q_DISABLE_COPY(RenderSession);

    static const QString SessionContextProperty;

    QUrl m_sourceUrl;
    QPointer<QQuickItem> m_loadedItem;
    Rational m_frameRate = DefaultFrameRate;
    int m_sampleRate = DefaultSampleRate;
    QAudioFormat m_outputAudioFormat;
    Interval<microseconds> m_currentRenderTime;
    int m_frameCount = 1;
    bool m_sessionEnded = false;
    int m_pauseRendering = 0;
    QAudioBuffer m_silentOutputAudioBuffer;
    std::unique_ptr<AudioRenderer> m_rootAudioRenderer;
    std::unique_ptr<AnimationDriver> m_animationDriver;
    bool m_isRenderEventPosted = false;
    bool m_isResumingRender = false;
};

class RenderSessionAttached : public QObject {
    Q_OBJECT
    Q_PROPERTY(RenderSession* session READ session CONSTANT)
    QML_ANONYMOUS
public:
    using QObject::QObject;

    explicit RenderSessionAttached(RenderSession* session, QObject* parent = nullptr)
        : QObject(parent)
        , m_session(session) {};
    RenderSessionAttached(RenderSessionAttached&&) = delete;
    RenderSessionAttached& operator=(RenderSessionAttached&&) = delete;
    ~RenderSessionAttached() override = default;

    RenderSession* session() const { return m_session; };

private:
    Q_DISABLE_COPY(RenderSessionAttached);
    RenderSession* m_session = nullptr;
};