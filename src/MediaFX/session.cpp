// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "session.h"
#include "animation.h"
#include "audio_renderer.h"
#include "media_manager.h"
#include "output_format.h"
#include "render_control.h"
#include "util.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QSGRendererInterface>
#include <QUrl>
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QQuickGraphicsConfiguration>
#endif

const QEvent::Type renderEventType = static_cast<const QEvent::Type>(QEvent::registerEventType());

Session::Session(const OutputFormat& outputFormat, const QUrl& url, bool exitOnWarning, QObject* parent)
    : QObject(parent)
    , exitOnWarning(exitOnWarning)
    , animationDriver(new AnimationDriver(frameRateToFrameDuration<microseconds>(outputFormat.frameRate()), this))
    , renderControl(std::make_unique<RenderControl>())
    , quickView(std::make_unique<QQuickView>(QUrl(), renderControl.get()))
{
    animationDriver->install();
    // Enables Qt.exit(0) in QML
    connect(quickView->engine(), &QQmlEngine::exit, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

#ifdef MEDIAFX_ENABLE_VULKAN
    if (quickView->rendererInterface()->graphicsApi() == QSGRendererInterface::Vulkan) {
        vulkanInstance.setExtensions(QQuickGraphicsConfiguration::preferredInstanceExtensions());
        vulkanInstance.create();
    }
#endif

    manager = std::make_unique<MediaManager>(outputFormat, quickView.get());
    connect(manager.get(), &MediaManager::renderingPausedChanged, this, &Session::onRenderingPausedChanged);

    quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView->resize(outputFormat.frameSize());
    connect(quickView.get(), &QQuickView::statusChanged, this, &Session::onQuickViewStatusChanged);
    connect(quickView->engine(), &QQmlEngine::warnings, this, &Session::onEngineWarnings);

#ifdef MEDIAFX_ENABLE_VULKAN
    if (quickView->rendererInterface()->graphicsApi() == QSGRendererInterface::Vulkan) {
        if (!vulkanInstance.isValid()) {
            qCritical() << "Invalid Vulkan instance";
            return;
        }
        quickView->setVulkanInstance(&vulkanInstance);
    }
#endif
    if (!renderControl->install(quickView.get())) {
        qCritical() << "Failed to install QQuickRenderControl";
        return;
    }
    quickView->setSource(url);
    if (quickView->status() == QQuickView::Error)
        return;
    m_isValid = true;
}

Session::~Session()
{
    animationDriver->uninstall();
}

void Session::postRenderEvent()
{
    if (m_isRenderEventPosted)
        return;
    m_isRenderEventPosted = true;
    QCoreApplication::postEvent(this, new QEvent(renderEventType)); // NOLINT(cppcoreguidelines-owning-memory)
}

void Session::onQuickViewStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Error) {
        emit quickView->engine()->exit(1);
    } else if (status == QQuickView::Ready) {
        quickView->rootObject()->setEnabled(false);
        postRenderEvent();
    }
}

void Session::onEngineWarnings(const QList<QQmlError>& warnings)
{
    // Warnings are logged
    if (exitOnWarning)
        emit quickView->engine()->exit(1);
}

bool Session::event(QEvent* event)
{
    if (event->type() == renderEventType) {
        m_isRenderEventPosted = false;
        render();
        return true;
    }
    return QObject::event(event);
}

const QAudioBuffer& Session::silentOutputAudioBuffer()
{
    if (!m_silentOutputAudioBuffer.isValid()) {
        m_silentOutputAudioBuffer = manager->createOutputAudioBuffer();
    }
    return m_silentOutputAudioBuffer;
}

void Session::onRenderingPausedChanged()
{
    if (!manager->isRenderingPaused())
        postRenderEvent();
}

void Session::render()
{
    if (manager->isRenderingPaused())
        return;
    if (!m_isResumingRender)
        manager->render();
    if (manager->isRenderingPaused()) {
        m_isResumingRender = true;
        return;
    }
    m_isResumingRender = false;

    QByteArray videoData = renderControl->renderVideoFrame();
    QAudioBuffer audioBuffer = manager->audioRenderer()->mix();
    if (!audioBuffer.isValid())
        audioBuffer = silentOutputAudioBuffer();
    manager->nextRenderTime();

    emit frameReady(audioBuffer, videoData);

    if (manager->isFinishedEncoding()) {
        emit sessionFinished();
        // Exit 0, the above slot should have exited with an error if necessary
        emit quickView->engine()->exit(0);
        return;
    }

    animationDriver->advance();

    postRenderEvent();
}