// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interval.h"
#include "render_context.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QObject>
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
extern "C" {
#include <libavutil/rational.h>
}
class AnimationDriver;
class AudioRenderer;
using namespace std::chrono;

class RenderSession : public QObject {
    Q_OBJECT
    Q_PROPERTY(IntervalGadget currentRenderTime READ currentRenderTime NOTIFY currentRenderTimeChanged FINAL)
    Q_PROPERTY(RenderContext renderContext READ renderContext CONSTANT)
    QML_ELEMENT
    QML_SINGLETON

public:
    RenderSession(QObject* parent = nullptr);
    RenderSession(RenderSession&&) = delete;
    RenderSession& operator=(RenderSession&&) = delete;
    ~RenderSession() override;

    void initialize(const RenderContext& renderContext);

    const RenderContext& renderContext() const { return *m_renderContext.get(); }

    Q_INVOKABLE IntervalGadget createInterval(qint64 start, qint64 end) const
    {
        return IntervalGadget(start, end);
    };

    const AVRational& outputFrameRate() const { return renderContext().frameRate(); }
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

signals:
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

private:
    Q_DISABLE_COPY(RenderSession);

    std::unique_ptr<RenderContext> m_renderContext;
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
