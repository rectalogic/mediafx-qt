// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioBuffer>
#include <QList>
#include <QObject>
#include <QtQmlIntegration>

class AudioRenderer : public QObject {
    Q_OBJECT
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(AudioRenderer* nextRenderer READ nextRenderer WRITE setNextRenderer NOTIFY nextRendererChanged FINAL)
    QML_ELEMENT

signals:
    void volumeChanged();
    void nextRendererChanged();

public:
    using QObject::QObject;

    explicit AudioRenderer(QObject* parent = nullptr);
    AudioRenderer(bool isRoot, QObject* parent = nullptr);
    AudioRenderer(AudioRenderer&&) = delete;
    AudioRenderer(const AudioRenderer&) = delete;
    AudioRenderer& operator=(AudioRenderer&&) = delete;
    AudioRenderer& operator=(const AudioRenderer&) = delete;
    ~AudioRenderer() override;

    float volume() const { return m_volume; };
    void setVolume(float volume);

    AudioRenderer* nextRenderer() const { return m_nextRenderer; };
    void setNextRenderer(AudioRenderer* nextRenderer);

    void addAudioBuffer(QAudioBuffer& audioBuffer);
    QAudioBuffer mix();

private:
    AudioRenderer* nextRendererInternal() const;
    void addParentRenderer(AudioRenderer* parent);
    void removeParentRenderer(AudioRenderer* parent);

    float m_volume = 1.0;
    QList<QAudioBuffer> audioBuffers;
    AudioRenderer* m_nextRenderer = nullptr;
    QList<AudioRenderer*> m_parentRenderers;
};
