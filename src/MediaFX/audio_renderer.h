// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioBuffer>
#include <QList>
#include <QObject>
#include <QQmlParserStatus>
#include <QtQmlIntegration>

class AudioRenderer : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(AudioRenderer* upstreamRenderer READ upstreamRenderer WRITE setUpstreamRenderer NOTIFY upstreamRendererChanged FINAL)
    QML_ELEMENT

signals:
    void volumeChanged();
    void upstreamRendererChanged();

public:
    using QObject::QObject;

    explicit AudioRenderer(QObject* parent = nullptr);
    AudioRenderer(AudioRenderer&&) = delete;
    AudioRenderer& operator=(AudioRenderer&&) = delete;
    ~AudioRenderer() override;

    float volume() const { return m_volume; };
    void setVolume(float volume);

    AudioRenderer* upstreamRenderer() const { return m_upstreamRenderer; };
    void setUpstreamRenderer(AudioRenderer* upstreamRenderer);

    void addAudioBuffer(QAudioBuffer audioBuffer);
    QAudioBuffer mix();

protected:
    void classBegin() override {};
    void componentComplete() override;

private:
    Q_DISABLE_COPY(AudioRenderer);

    AudioRenderer* rootAudioRenderer();
    void addDownstreamRenderer(AudioRenderer* downstreamRenderer);
    void removeDownstreamRenderer(AudioRenderer* downstreamRenderer);

    float m_volume = 1.0;
    QList<QAudioBuffer> audioBuffers;
    AudioRenderer* m_upstreamRenderer = nullptr;
    QList<AudioRenderer*> m_downstreamRenderers;
    AudioRenderer* m_rootAudioRenderer = nullptr;
};
