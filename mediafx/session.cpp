// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "session.h"
#include "mediafx.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickView>
#include <QUrl>
#include <errno.h>
#include <string.h>
#include <unistd.h>

QEvent::Type Session::renderEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

Session::Session(QUrl& url, QSize& size, qint64 frameDuration)
    : QObject()
    , m_frameDuration(frameDuration)
    , frameTime(0, frameDuration)
    , quickView(QUrl(), &renderControl)
{
    connect(this, &Session::exitApp, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    MediaFXForeign::s_singletonInstance = new MediaFX(this, this);

    quickView.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView.resize(size);
    if (!renderControl.install(quickView, size)) {
        qCritical() << "Failed to install QQuickRenderControl";
        emit exitApp(1);
        return;
    }
    connect(&quickView, &QQuickView::statusChanged, this, &Session::quickViewStatusChanged);
    connect(quickView.engine(), &QQmlEngine::warnings, this, &Session::engineWarnings);

    // Workaround https://bugreports.qt.io/browse/QTBUG-118165 - we can't just use qmlTypeId()
    // This will call MediaFXForeign::create() which will initialize typeId
    mediaFX = quickView.engine()->singletonInstance<MediaFX*>("stream.mediafx", "MediaFX");

    quickView.setSource(url);
}

void Session::quickViewStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Error) {
        emit exitApp(1);
    } else if (status == QQuickView::Ready) {
        render();
    }
}

void Session::engineWarnings(const QList<QQmlError>& warnings)
{
    for (const QQmlError& warning : warnings) {
        qWarning() << warning.toString();
    }
}

bool Session::event(QEvent* event)
{
    if (event->type() == renderEventType) {
        render();
        return true;
    }
    return QObject::event(event);
}

void Session::render()
{
    if (mediaFX->renderVideoFrame(frameTime)) {
        auto frameData = renderControl.renderVideoFrame();

        /**** XXX ****/
        size_t bytesIO = 0;
        auto size = frameData.size();
        const auto data = frameData.constData();
        while (bytesIO < size) {
            ssize_t n = write(STDOUT_FILENO, data + bytesIO, size - bytesIO);
            if (n == -1) {
                qCritical() << "write failed: " << strerror(errno);
                return;
            }
            bytesIO = bytesIO + n;
        }
        /**** XXX ****/

        frameTime = frameTime.translated(frameDuration());
    }

    // XXX need to know when we're done - how do we determine total duration?
    // XXX QML could signal a mediaFX slot when done
    QCoreApplication::postEvent(this, new QEvent(renderEventType));
}