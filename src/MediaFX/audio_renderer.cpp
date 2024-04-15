// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_renderer.h"
#include "render_session.h"
#include <QAudioFormat>
#include <QObject>
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
AudioRenderer::AudioRenderer(QObject* parent)
    : QObject(parent)
{
}

AudioRenderer::~AudioRenderer() = default;

void AudioRenderer::classBegin()
{
    // We default to the root renderer as upstreamRenderer, unless we are root.
    // The root renderer is created in C++ so classBegin() is not called.
    upstreamRendererInternal()->addDownstreamRenderer(this);
}

AudioRenderer* AudioRenderer::rootAudioRenderer()
{
    if (!m_rootAudioRenderer)
        m_rootAudioRenderer = qmlEngine(this)->singletonInstance<RenderSession*>("MediaFX", "RenderSession")->rootAudioRenderer();
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
        // Don't allow it to be set on the root renderer
        if (this == rootAudioRenderer())
            return;
        // Avoid circular dependencies
        auto renderer = upstreamRenderer;
        while (renderer != nullptr) {
            if (renderer == this) {
                qmlWarning(this) << "Circular dependency in AudioRenderer";
                return;
            }
            renderer = renderer->upstreamRenderer();
        }
        upstreamRendererInternal()->removeDownstreamRenderer(this);
        m_upstreamRenderer = upstreamRenderer;
        upstreamRendererInternal()->addDownstreamRenderer(this);
        emit upstreamRendererChanged();
    }
}

AudioRenderer* AudioRenderer::upstreamRendererInternal()
{
    return m_upstreamRenderer ? m_upstreamRenderer : rootAudioRenderer();
}

void AudioRenderer::addDownstreamRenderer(AudioRenderer* parent)
{
    m_downstreamRenderers.append(parent);
}

void AudioRenderer::removeDownstreamRenderer(AudioRenderer* parent)
{
    m_downstreamRenderers.removeAll(parent);
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

    // Mix each parent and add their valid buffers
    for (auto parent : m_downstreamRenderers) {
        QAudioBuffer buffer = parent->mix();
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
