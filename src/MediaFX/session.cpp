// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "session.h"
#include "animation.h"
#include "encoder.h"
#include "media_manager.h"
#include "render_control.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickItem>
#include <QQuickView>
#include <QSGRendererInterface>
#include <QUrl>
#include <errno.h>
#include <ffms.h>
#include <string.h>
#include <unistd.h>
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QQuickGraphicsConfiguration>
#endif

QEvent::Type Session::renderEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

Session::Session(Encoder* encoder, bool exitOnWarning, QObject* parent)
    : QObject(parent)
    , exitOnWarning(exitOnWarning)
    , encoder(encoder)
    , m_frameDuration(encoder->frameRate().toFrameDuration())
    , animationDriver(new AnimationDriver(m_frameDuration, this))
    , renderControl(nullptr)
    , quickView(nullptr)
{
    FFMS_Init(0, 0);

    animationDriver->install();
    renderControl.reset(new RenderControl());
    quickView.reset(new QQuickView(QUrl(), renderControl.get()));
    // Enables Qt.exit(0) in QML
    connect(quickView->engine(), &QQmlEngine::exit, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

#ifdef MEDIAFX_ENABLE_VULKAN
    if (quickView->rendererInterface()->graphicsApi() == QSGRendererInterface::Vulkan) {
        vulkanInstance.setExtensions(QQuickGraphicsConfiguration::preferredInstanceExtensions());
        vulkanInstance.create();
    }
#endif

    manager = new MediaManager(m_frameDuration, quickView.get(), this);
    MediaManagerForeign::s_singletonInstance = manager;

    quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView->resize(encoder->frameSize().toSize());
    connect(quickView.get(), &QQuickView::statusChanged, this, &Session::quickViewStatusChanged);
    connect(quickView->engine(), &QQmlEngine::warnings, this, &Session::engineWarnings);
}

Session::~Session()
{
    animationDriver->uninstall();
    FFMS_Deinit();
}

bool Session::initialize(const QUrl& url)
{
#ifdef MEDIAFX_ENABLE_VULKAN
    if (quickView->rendererInterface()->graphicsApi() == QSGRendererInterface::Vulkan) {
        if (!vulkanInstance.isValid()) {
            qCritical() << "Invalid Vulkan instance";
            return false;
        }
        quickView->setVulkanInstance(&vulkanInstance);
    }
#endif
    if (!renderControl->install(quickView.get())) {
        qCritical() << "Failed to install QQuickRenderControl";
        return false;
    }
    quickView->setSource(url);
    if (quickView->status() == QQuickView::Error)
        return false;
    else
        return true;
}

void Session::quickViewStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Error) {
        emit quickView->engine()->exit(1);
    } else if (status == QQuickView::Ready) {
        quickView->rootObject()->setEnabled(false);
        QCoreApplication::postEvent(this, new QEvent(renderEventType));
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

int write(int fd, qsizetype size, const char* data)
{
    size_t bytesIO = 0;
    while (bytesIO < size) {
        ssize_t n = write(fd, data + bytesIO, size - bytesIO);
        if (n == -1) {
            qCritical() << "write failed: " << strerror(errno);
            return -1;
        }
        bytesIO = bytesIO + n;
    }
    return size;
}

void Session::render()
{
    manager->render();

    auto frameData = renderControl->renderVideoFrame();

    if (write(encoder->videofd(), frameData.size(), frameData.constData()) == -1)
        return;

    emit manager->frameRendered();

    if (manager->isFinishedEncoding()) {
        emit quickView->engine()->exit(0);
        return;
    }

    animationDriver->advance();

    QCoreApplication::postEvent(this, new QEvent(renderEventType));
}