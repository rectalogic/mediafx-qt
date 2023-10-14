// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "session.h"
#include "mediafx.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickView>
#include <QSize>
class QUrl;

Session::Session(QUrl& url, QSize size)
    : QObject()
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
        // XXX kick off state machine
    }
}

void Session::engineWarnings(const QList<QQmlError>& warnings)
{
    for (const QQmlError& warning : warnings) {
        qWarning() << warning.toString();
    }
}
