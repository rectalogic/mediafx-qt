// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_renderer.h"
#include "render_session.h"
#include <QAudioFormat>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QQmlInfo>
#include <QmlTypeAndRevisionsRegistration>
#include <QtCore>

/*!
    \qmltype AudioRenderer
    //! \instantiates AudioRenderer
    \inqmlmodule MediaFX

    \brief Renders audio for a \l MediaClip.
*/

void AudioRenderer::componentComplete()
{
    if (!m_upstreamRenderer) {
        // We default to the root renderer as upstreamRenderer, unless we are root.
        // The root renderer is created in C++ so classBegin() is not called.
        setUpstreamRenderer(rootAudioRenderer());
    }
}

AudioRenderer* AudioRenderer::rootAudioRenderer()
{
    if (!m_rootAudioRenderer) {
        RenderSession* renderSession = RenderSession::findSession(this);
        if (renderSession) {
            m_rootAudioRenderer = renderSession->rootAudioRenderer();
        } else {
            qmlWarning(this) << "AudioRenderer could not find renderSession in context";
            emit qmlEngine(this)->exit(1);
        }
    }
    return m_rootAudioRenderer;
}

/*!
    \qmlproperty real AudioRenderer::volume

    Audio volume.
    The volume is scaled linearly from 0.0 (silence) to 1.0 (full volume).
    The default volume is 1.0.
*/
void AudioRenderer::setVolume(float volume)
{
    if (volume != m_volume) {
        if (volume >= 0 && volume <= 1.0) {
            m_volume = volume;
            emit volumeChanged();
        } else {
            qmlWarning(this) << "Invalid volume, must be 0 <= volume <= 1.0";
        }
    }
}

/*!
    \qmlproperty real AudioRenderer::upstreamRenderer

    The upstream AudioRenderer in the chain. If not set then the root AudioRenderer is used.
*/
void AudioRenderer::setUpstreamRenderer(AudioRenderer* upstreamRenderer)
{
    if (upstreamRenderer != m_upstreamRenderer) {
        // Avoid circular dependencies
        auto renderer = upstreamRenderer;
        while (renderer != nullptr) {
            if (renderer == this) {
                qmlWarning(this) << "Circular dependency in AudioRenderer";
                return;
            }
            renderer = renderer->upstreamRenderer();
        }
        if (m_upstreamRenderer)
            m_upstreamRenderer->removeDownstreamRenderer(this);
        m_upstreamRenderer = upstreamRenderer;
        if (m_upstreamRenderer)
            m_upstreamRenderer->addDownstreamRenderer(this);
        emit upstreamRendererChanged();
    }
}

void AudioRenderer::addDownstreamRenderer(AudioRenderer* downstreamRenderer)
{
    m_downstreamRenderers.append(downstreamRenderer);
}

void AudioRenderer::removeDownstreamRenderer(AudioRenderer* downstreamRenderer)
{
    m_downstreamRenderers.removeAll(downstreamRenderer);
}

void AudioRenderer::addAudioBuffer(QAudioBuffer audioBuffer)
{
    audioBuffers.append(audioBuffer);
}

QAudioBuffer AudioRenderer::mix()
{
    // No sound
    if (volume() == 0.0) {
        audioBuffers.clear();
        return QAudioBuffer();
    }

    // Mix each downstream and add their valid buffers
    for (auto downstream : m_downstreamRenderers) {
        QAudioBuffer buffer = downstream->mix();
        if (buffer.isValid())
            audioBuffers.append(buffer);
    }

    // No sound
    if (audioBuffers.isEmpty()) {
        return QAudioBuffer();
    }

    QAudioBuffer outputBuffer = audioBuffers.takeFirst();
    // Mix buffers together
    if (!audioBuffers.isEmpty()) {
        for (int i = 0; i < outputBuffer.sampleCount(); i++) {
            for (const auto& buffer : audioBuffers) {
                Q_ASSERT(buffer.format() == outputBuffer.format() && buffer.frameCount() == outputBuffer.frameCount());
                outputBuffer.data<float>()[i] += buffer.constData<float>()[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            }
        }
    }
    // Apply volume to mixed buffer
    if (volume() != 1.0) {
        for (int i = 0; i < outputBuffer.sampleCount(); i++) {
            outputBuffer.data<float>()[i] *= volume(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
    }
    audioBuffers.clear();
    return outputBuffer;
}
