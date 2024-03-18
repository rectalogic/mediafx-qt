// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAudioBuffer>
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
class MediaManager;
class OutputFormat;
class QQmlError;
class RenderControl;
using namespace std::chrono;

class Session : public QObject {
    Q_OBJECT

public:
    Session(const OutputFormat& outputFormat, const QUrl& url, bool exitOnWarning, QObject* parent = nullptr);
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;
    ~Session() override;

    bool isValid() const { return m_isValid; }

    void render();

    bool event(QEvent* event) override;

signals:
    void frameReady(const QAudioBuffer& audioBuffer, const QByteArray& videoData);
    void sessionFinished();

protected:
    void postRenderEvent();

private slots:
    void onQuickViewStatusChanged(QQuickView::Status status);
    void onEngineWarnings(const QList<QQmlError>& warnings);
    void onRenderingPausedChanged();

private:
    Q_DISABLE_COPY(Session);

    const QAudioBuffer& silentOutputAudioBuffer();

    bool m_isValid = false;
    QAudioBuffer m_silentOutputAudioBuffer;
    bool exitOnWarning;
    AnimationDriver* animationDriver;
#ifdef MEDIAFX_ENABLE_VULKAN
    QVulkanInstance vulkanInstance;
#endif
    std::unique_ptr<RenderControl> renderControl;
    std::unique_ptr<QQuickView> quickView;
    std::unique_ptr<MediaManager> manager;
    bool m_isRenderEventPosted = false;
    bool m_isResumingRender = false;
};