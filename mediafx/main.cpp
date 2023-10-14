// Copyright (c) 2023 Andrew Wason, All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mediafx.h"
#include "render_control.h"
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QQuickView>
#include <QStringList>
#include <QUrl>
#include <stdlib.h>

int main(int argc, char* argv[])
{
#ifdef TARGET_OS_MAC
    putenv(
        const_cast<char*>("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM=1"));
#endif

    QGuiApplication app(argc, argv);

    app.setOrganizationDomain("mediafx.stream");
    app.setOrganizationName("mediaFX");
    app.setApplicationName("mediaFX");

    // XXX need encoder settings which include frame size
    QSize size(640, 360);
    QCommandLineParser parser;
    parser.setApplicationDescription("mediaFX");
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();
    parser.addPositionalArgument("source", "QML source URL.");

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        qCritical("Missing required source url");
        exit(1);
    }
    QUrl url(args.at(0));

    RenderControl renderControl;
    QQuickView quickView(QUrl(), &renderControl);
    quickView.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    renderControl.install(quickView, size);
    MediaFX* mediaFX = quickView.engine()->singletonInstance<MediaFX*>(MediaFX::typeId);

    // XXX load qml
    // XXX connect to quickview errors, loadfinished etc.

    return app.exec();
}
