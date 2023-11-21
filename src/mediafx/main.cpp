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
#include <QMessageLogContext>
#include <QSize>
#include <QStringList>
#include <QUrl>
#include <QtTypes>
#include <stdio.h>
#ifdef WEBENGINEQUICK
#include <QtWebEngineQuick>
#endif

#define qSL QStringLiteral

qint64 computeFrameDuration(const QString& frameRate)
{
    auto frameRateF = frameRate.toDouble();
    if (frameRateF <= 0) {
        float num = 0, den = 0;
        if (sscanf(frameRate.toUtf8().data(), "%f/%f", &num, &den) == 2 && den > 0) {
            frameRateF = num / den;
        }
    }
    qint64 frameDuration = 0;
    if (frameRateF > 0) {
        return 1000000 / frameRateF; // frame duration in microseconds
    } else {
        return -1;
    }
}

QSize parseFrameSize(const QString& frameSize)
{
    QSize size;
    if (sscanf(frameSize.toUtf8().data(), "%dx%d", &size.rwidth(), &size.rheight()) == 2 && size.width() > 0 && size.height() > 0) {
        return size;
    }
    return QSize();
}

int main(int argc, char* argv[])
{
#ifdef TARGET_OS_MAC
    putenv(
        const_cast<char*>("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM=1"));
#endif

#ifdef WEBENGINEQUICK
#if !defined(QT_NO_OPENGL)
    // https://doc.qt.io/qt-6/qml-qtwebengine-webengineview.html#rendering-to-opengl-surface
    // https://doc.qt.io/qt-6/qtwebengine-overview.html#embedding-web-content-into-qt-quick-applications
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
    QtWebEngineQuick::initialize();
#endif

    QGuiApplication app(argc, argv);

    app.setOrganizationDomain(qSL("mediafx.stream"));
    app.setOrganizationName(qSL("mediaFX"));
    app.setApplicationName(qSL("mediaFX"));

    QCommandLineParser parser;
    parser.setApplicationDescription(qSL("mediaFX"));
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();
    parser.addOption({ { qSL("f"), qSL("frameRate") }, qSL("Output frames per second, can be float or rational e.g. 30000/1001"), qSL("frameRate"), qSL("30") });
    parser.addOption({ { qSL("s"), qSL("size") }, qSL("Frame size, WxH"), qSL("size"), qSL("640x360") });
    parser.addPositionalArgument(qSL("source"), qSL("QML source URL."));

    parser.process(app);

    qint64 frameDuration = computeFrameDuration(parser.value(qSL("frameRate")));
    if (frameDuration <= 0)
        parser.showHelp(1);

    QSize size = parseFrameSize(parser.value(qSL("size")));
    if (size.isEmpty())
        parser.showHelp(1);

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
