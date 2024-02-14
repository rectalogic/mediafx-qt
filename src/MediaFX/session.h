// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioBuffer>
#include <QEvent>
#include <QList>
#include <QObject>
#include <QQuickView>
#include <QtCore>
#include <chrono>
#include <memory>
#include <qtgui-config.h>
#if (QT_CONFIG(vulkan) && __has_include(<vulkan/vulkan.h>))
#define MEDIAFX_ENABLE_VULKAN
#endif
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QVulkanInstance>
#endif
class AnimationDriver;
class Encoder;
class MediaManager;
class QQmlError;
class RenderControl;
using namespace std::chrono;

class Session : public QObject {
    Q_OBJECT

public:
    Session(Encoder* encoder, bool exitOnWarning, QObject* parent = nullptr);
    Session(Session&&) = delete;
    Session(const Session&) = delete;
    Session& operator=(Session&&) = delete;
    Session& operator=(const Session&) = delete;
    ~Session() override;

    bool initialize(const QUrl& url);

    microseconds outputVideoFrameDuration() const noexcept { return m_outputVideoFrameDuration; };

    void render();

    bool event(QEvent* event) override;

private slots:
    void quickViewStatusChanged(QQuickView::Status status);
    void engineWarnings(const QList<QQmlError>& warnings);

private:
    const QAudioBuffer& silentOutputAudioBuffer();

    QAudioBuffer m_silentOutputAudioBuffer;
    bool exitOnWarning;
    Encoder* encoder;
    microseconds m_outputVideoFrameDuration;
    AnimationDriver* animationDriver;
#ifdef MEDIAFX_ENABLE_VULKAN
    QVulkanInstance vulkanInstance;
#endif
    std::unique_ptr<RenderControl> renderControl;
    std::unique_ptr<QQuickView> quickView;
    std::unique_ptr<MediaManager> manager;
};