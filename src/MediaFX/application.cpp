// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include <QCoreApplication>
#include <Qt>
#ifdef WEBENGINEQUICK
#include <QtWebEngineQuick>
#endif

void initializeMediaFX()
{
#ifdef WEBENGINEQUICK
#if !defined(QT_NO_OPENGL)
    // https://doc.qt.io/qt-6/qml-qtwebengine-webengineview.html#rendering-to-opengl-surface
    // https://doc.qt.io/qt-6/qtwebengine-overview.html#embedding-web-content-into-qt-quick-applications
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
    QtWebEngineQuick::initialize();
#endif
}
