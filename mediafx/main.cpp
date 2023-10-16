// Copyright (c) 2023 Andrew Wason, All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "session.h"
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QList>
#include <QMessageLogContext>
#include <QSize>
#include <QStringList>
#include <QUrl>
#include <QtTypes>
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

    // XXX need encoder settings which include frame size, frame duration
    QSize size(640, 360);
    qint64 frameDuration = 33333;

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

    Session session(url, size, frameDuration);

    return app.exec();
}
