// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_renderer.h"
#include "media_manager.h"
#include <QAudioFormat>
#include <QObject>
#include <QQmlInfo>
#include <QtCore>

/*!
    \qmltype AudioRenderer
    //! \instantiates AudioRenderer
    \inqmlmodule MediaFX

    \brief Renders audio for a \l MediaClip.
*/
AudioRenderer::AudioRenderer(QObject* parent)
    : AudioRenderer(false, parent)
{
}

AudioRenderer::AudioRenderer(bool isRoot, QObject* parent)
    : QObject(parent)
{
    // We default to the root renderer as nextRenderer, unless we are root
    if (!isRoot)
        nextRendererInternal()->addParentRenderer(this);
}

AudioRenderer::~AudioRenderer() = default;

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
    \qmlproperty real AudioRenderer::nextRenderer

    The next AudioRenderer in the chain. If not set then the root AudioRenderer is used.
*/
void AudioRenderer::setNextRenderer(AudioRenderer* nextRenderer)
{
    if (nextRenderer != m_nextRenderer) {
        // Don't allow it to be set on the root renderer
        if (this == MediaManager::singletonInstance()->audioRenderer())
            return;
        // Avoid circular dependencies
        auto renderer = nextRenderer;
        while (renderer != nullptr) {
            if (renderer == this) {
                qmlWarning(this) << "Circular dependency in AudioRenderer";
                return;
            }
            renderer = renderer->nextRenderer();
        }
        nextRendererInternal()->removeParentRenderer(this);
        m_nextRenderer = nextRenderer;
        nextRendererInternal()->addParentRenderer(this);
        emit nextRendererChanged();
    }
}

AudioRenderer* AudioRenderer::nextRendererInternal() const
{
    return m_nextRenderer ? m_nextRenderer : MediaManager::singletonInstance()->audioRenderer();
}

void AudioRenderer::addParentRenderer(AudioRenderer* parent)
{
    m_parentRenderers.append(parent);
}

void AudioRenderer::removeParentRenderer(AudioRenderer* parent)
{
    m_parentRenderers.removeAll(parent);
}

void AudioRenderer::addAudioBuffer(QAudioBuffer& audioBuffer)
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
    for (auto parent : m_parentRenderers) {
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
