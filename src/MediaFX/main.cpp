// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "encoder.h"
#ifdef EVENTLOGGER
#include "event_logger.h"
#endif
#include "session.h"
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QString>
#include <QStringList>
#include <QStringLiteral>
#include <QUrl>

const auto ffmpegPreamble = qSL(" -hide_banner -loglevel warning -f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -i async:pipe:${MEDIAFX_VIDEOFD} ");

int main(int argc, char* argv[])
{
#ifdef TARGET_OS_MAC
    putenv(
        const_cast<char*>("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM=1"));
#endif

    initializeMediaFX();
    QGuiApplication app(argc, argv);

#ifdef EVENTLOGGER
    app.installEventFilter(new EventLogger(&app));
#endif

    app.setOrganizationDomain(qSL("mediafx.org"));
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
    parser.addOption({ { qSL("w"), qSL("exitOnWarning") }, qSL("Exit on QML warnings") });
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
        if (command == qSL("ffplay")) {
            // XXX need to handle audio eventually
            command = qSL("ffplay -autoexit -infbuf") + ffmpegPreamble;
        } else if (command == qSL("ffmpeg") || command.startsWith(qSL("ffmpeg:"))) {
            if (!parser.isSet(qSL("output"))) {
                parser.showHelp(1);
            }
            if (command == qSL("ffmpeg:lossless")) {
                command = qSL("ffmpeg") + ffmpegPreamble + qSL("-f mp4 -codec:v libx264 -preset veryslow -qp 0 -y ${MEDIAFX_OUTPUT}");
            } else if (command == qSL("ffmpeg:mov")) {
                command = qSL("ffmpeg") + ffmpegPreamble + qSL("-f mov -codec:v rawvideo -pix_fmt uyvy422 -vtag yuvs -y ${MEDIAFX_OUTPUT}");
            } else if (command == qSL("ffmpeg")) {
                // Let ffmpeg choose codec/format based on filename suffix
                command = qSL("ffmpeg") + ffmpegPreamble + qSL("-y ${MEDIAFX_OUTPUT}");
            } else {
                // Treat arguments as encoding parameters
                command = qSL("ffmpeg") + ffmpegPreamble + command.last(command.size() - qSL("ffmpeg:").size()) + qSL(" -y ${MEDIAFX_OUTPUT}");
            }
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
    Session session(&encoder, parser.isSet(qSL("exitOnWarning")));
    if (!session.initialize(url)) {
        qCritical("Failed to initialize session");
        return 1;
    }
    return app.exec();
}
