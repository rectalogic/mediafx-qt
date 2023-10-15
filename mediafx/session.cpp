// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "session.h"
#include "mediafx.h"
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickView>
#include <QSize>
class QUrl;

QEvent::Type Session::renderEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

Session::Session(QUrl& url, QSize& size, qint64 frameDuration)
    : QObject()
    , frameDuration(frameDuration)
    , frameTime(0, frameDuration)
    , quickView(url, &renderControl)
{
    quickView.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView.resize(size);
    renderControl.install(quickView, size);
    mediaFX = quickView.engine()->singletonInstance<MediaFX*>(MediaFX::typeId);

    connect(&quickView, &QQuickView::statusChanged, this, &Session::quickViewStatusChanged);
    connect(quickView.engine(), &QQmlEngine::warnings, this, &Session::engineWarnings);
}

void Session::quickViewStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Error) {
        engineWarnings(quickView.errors());
        QCoreApplication::exit(1);
    } else if (status == QQuickView::Ready) {
        renderFrame();
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
        renderFrame();
        return true;
    }
    return QObject::event(event);
}

void Session::renderFrame()
{
    if (mediaFX->renderVideoFrame(frameTime)) {
        auto frameData = renderControl.renderVideoFrame();
        frameTime = frameTime.translated(frameDuration);
    }

    // XXX should we post this from Clip when new frames are available? but what about producers
    // XXX need to know when we're done - how do we determine total duration?
    QCoreApplication::postEvent(this, new QEvent(renderEventType));
}