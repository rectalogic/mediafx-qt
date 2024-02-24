// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "session.h"
#include "animation.h"
#include "audio_renderer.h"
#include "encoder.h"
#include "media_manager.h"
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

Session::Session(Encoder* encoder, const QUrl& url, bool exitOnWarning, QObject* parent)
    : QObject(parent)
    , exitOnWarning(exitOnWarning)
    , encoder(encoder)
    , animationDriver(new AnimationDriver(frameRateToFrameDuration<microseconds>(encoder->outputFrameRate()), this))
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

    manager = std::make_unique<MediaManager>(encoder->outputFrameRate(), encoder->outputSampleRate(), quickView.get());

    quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView->resize(encoder->outputFrameSize());
    connect(quickView.get(), &QQuickView::statusChanged, this, &Session::quickViewStatusChanged);
    connect(quickView->engine(), &QQmlEngine::warnings, this, &Session::engineWarnings);

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

void Session::quickViewStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Error) {
        emit quickView->engine()->exit(1);
    } else if (status == QQuickView::Ready) {
        quickView->rootObject()->setEnabled(false);
        QCoreApplication::postEvent(this, new QEvent(renderEventType)); // NOLINT(cppcoreguidelines-owning-memory)
    }
}

void Session::engineWarnings(const QList<QQmlError>& warnings)
{
    // Warnings are logged
    if (exitOnWarning)
        emit quickView->engine()->exit(1);
}

bool Session::event(QEvent* event)
{
    if (event->type() == renderEventType) {
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

void Session::render()
{
    manager->render();
    QByteArray videoData = renderControl->renderVideoFrame();
    QAudioBuffer audioBuffer = manager->audioRenderer()->mix();
    if (!audioBuffer.isValid())
        audioBuffer = silentOutputAudioBuffer();
    manager->nextRenderTime();

    if (!encoder->encode(audioBuffer, videoData))
        return;

    if (manager->isFinishedEncoding()) {
        int rc = 0;
        if (!encoder->finish())
            rc = 1;
        emit quickView->engine()->exit(rc);
        return;
    }

    animationDriver->advance();

    QCoreApplication::postEvent(this, new QEvent(renderEventType)); // NOLINT(cppcoreguidelines-owning-memory)
}