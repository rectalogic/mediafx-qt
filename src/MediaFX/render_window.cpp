// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#include "render_window.h"
#include "audio_renderer.h"
#include "render_control.h"
#include "render_session.h"
#include <QAudioBuffer>
#include <QByteArray>
#include <QDebug>
#include <QMessageLogContext>
#include <QQmlEngine>
#include <QQmlInfo>
#include <QQuickItem>
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QQuickGraphicsConfiguration>
#include <QSGRendererInterface>
#include <QVersionNumber>
#endif

RenderWindow::RenderWindow(RenderControl* renderControl)
    : QQuickWindow(renderControl)
    , m_renderControl(renderControl)
{
}

/*!
    \qmltype RenderWindow
    //! \instantiates RenderWindow
    \inqmlmodule MediaFX

    \brief Renders QML content into an offscreen buffer.
*/
RenderWindow::RenderWindow()
    : RenderWindow(new RenderControl())
{
#ifdef MEDIAFX_ENABLE_VULKAN
    if (rendererInterface()->graphicsApi() == QSGRendererInterface::Vulkan) {
        m_vulkanInstance.setExtensions(QQuickGraphicsConfiguration::preferredInstanceExtensions());
        m_vulkanInstance.setApiVersion(QVersionNumber(1, 0));
        m_vulkanInstance.create();
        if (!m_vulkanInstance.isValid()) {
            qCritical() << "Invalid Vulkan instance";
            return;
        }
        setVulkanInstance(&m_vulkanInstance);
    }
#endif
    if (!m_renderControl->initialize()) {
        qCritical() << "Failed to initialize QQuickRenderControl";
        return;
    }
    m_isValid = true;
}

RenderWindow::~RenderWindow() = default;

void RenderWindow::componentComplete()
{
    // QQuickWindow does not resize contentItem
    // https://bugreports.qt.io/browse/QTBUG-55028
    contentItem()->setSize(size());

    if (!m_renderSession) {
        qmlWarning(this) << "RenderWindow renderSession is a required property";
        emit qmlEngine(this)->exit(1);
    }
}

void RenderWindow::setRenderSession(RenderSession* renderSession)
{
    if (renderSession != m_renderSession) {
        m_renderSession = renderSession;
        emit renderSessionChanged();
    }
}

void RenderWindow::render()
{
    if (!m_isValid) {
        emit qmlEngine(this)->exit(1);
        return;
    }
    QByteArray videoData = m_renderControl->renderVideoFrame();
    if (videoData.isNull()) {
        emit qmlEngine(this)->exit(1);
        return;
    }
    QAudioBuffer audioBuffer = renderSession()->rootAudioRenderer()->mix();
    if (!audioBuffer.isValid())
        audioBuffer = renderSession()->silentOutputAudioBuffer();

    emit frameReady(audioBuffer, videoData);
}