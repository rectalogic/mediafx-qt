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

#include "encoder.h"
#include "session.h"
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QString>
#include <QStringList>
#include <QStringLiteral>
#include <QUrl>

#ifdef WEBENGINEQUICK
#include <QtWebEngineQuick>
#endif

#define qSL QStringLiteral

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
    parser.addOption({ { qSL("f"), qSL("fps") }, qSL("Output frames per second, can be float or rational e.g. 30000/1001"), qSL("fps"), qSL("30") });
    parser.addOption({ { qSL("s"), qSL("size") }, qSL("Output video frame size, WxH"), qSL("size"), qSL("640x360") });
    parser.addOption({ { qSL("o"), qSL("output") }, qSL("Output filename"), qSL("output") });
    parser.addOption({ { qSL("c"), qSL("command") }, qSL("Encoder commandline"), qSL("command") });
    parser.addPositionalArgument(qSL("source"), qSL("QML source URL."));

    parser.process(app);

    Encoder::FrameRate frameRate = Encoder::FrameRate::parse(parser.value(qSL("fps")));
    if (frameRate.isEmpty())
        parser.showHelp(1);

    Encoder::FrameSize frameSize = Encoder::FrameSize::parse(parser.value(qSL("size")));
    if (frameSize.isEmpty())
        parser.showHelp(1);

    QString command;
    if (parser.isSet(qSL("command"))) {
        command = parser.value(qSL("command"));
        if (command == "ffplay") {
            // XXX need to handle audio eventually
            command = qSL("ffplay -autoexit -infbuf -f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -i pipe:${MEDIAFX_VIDEOFD}");
        } else if (command == "ffmpeg") {
            if (!parser.isSet(qSL("output"))) {
                parser.showHelp(1);
            }
            // XXX quicktime no longer supports qtrle
            command = qSL("ffmpeg -f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -i pipe:${MEDIAFX_VIDEOFD} -f mov -vcodec rawvideo -pix_fmt uyvy422 -vtag yuvs -framerate ${MEDIAFX_FRAMERATE} -y ${MEDIAFX_OUTPUT}");
        }
    }
    QString output;
    if (parser.isSet(qSL("output"))) {
        output = parser.value(qSL("output"));
    }

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        qCritical("Missing required source url");
        parser.showHelp(1);
    }
    QUrl url(args.at(0));

    Encoder encoder(frameSize, frameRate);
    if (!encoder.initialize(output, command)) {
        qCritical("Failed to initialize encoder");
        return 1;
    }
    Session session(&encoder);
    if (!session.initialize(url)) {
        qCritical("Failed to initialize session");
        return 1;
    }
    return app.exec();
}
