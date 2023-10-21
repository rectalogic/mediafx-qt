/*
 * Copyright (C) 2023 Andrew Wason
 *
 * This file is part of mediaFX.
 *
 * mediaFX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with mediaFX.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
    qint64 frameDuration = 33333; // XXX duration in microseconds

    QCommandLineParser parser;
    parser.setApplicationDescription("mediaFX");
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();
    parser.addPositionalArgument("source", "QML source URL.");

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        qCritical("Missing required source url");
        return 1;
    }
    QUrl url(args.at(0));

    Session session(size, frameDuration);
    if (!session.initialize(url)) {
        qCritical("Failed to initialize session");
        return 1;
    }
    return app.exec();
}
