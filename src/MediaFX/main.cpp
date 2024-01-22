// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "encoder.h"
#include "session.h"
#include <QByteArray>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <stdio.h>
#ifdef EVENTLOGGER
#include "event_logger.h"
#endif
using namespace Qt::Literals::StringLiterals;

const auto ffmpegPreamble = u" -hide_banner -loglevel warning -f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -i async:pipe:${MEDIAFX_VIDEOFD} "_s;
const auto ffplayPreset = u"ffplay -autoexit -infbuf"_s + ffmpegPreamble;
// Let ffmpeg choose codec/format based on filename suffix
const auto ffmpegPreset = u"ffmpeg"_s + ffmpegPreamble + u"-y ${MEDIAFX_OUTPUT}"_s;
const auto ffmpegLosslessPreset = u"ffmpeg"_s + ffmpegPreamble + u"-f mp4 -codec:v libx264 -preset veryslow -qp 0 -y ${MEDIAFX_OUTPUT}"_s;
const auto ffmpegCustomPreset = u"ffmpeg"_s + ffmpegPreamble + u"%1 -y ${MEDIAFX_OUTPUT}"_s;

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

    app.setOrganizationDomain(u"mediafx.org"_s);
    app.setOrganizationName(u"MediaFX"_s);
    app.setApplicationName(u"MediaFX"_s);

    QCommandLineParser parser;
    parser.setApplicationDescription(u"MediaFX\nCopyright (C) 2023-2024 Andrew Wason\nSPDX-License-Identifier: GPL-3.0-or-later"_s);
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();
    parser.addOption({ { u"f"_s, u"fps"_s }, u"Output frames per second, can be float or rational e.g. 30000/1001."_s, u"fps"_s, u"30"_s });
    parser.addOption({ { u"s"_s, u"size"_s }, u"Output video frame size, WxH."_s, u"size"_s, u"640x360"_s });
    parser.addOption({ { u"o"_s, u"output"_s }, u"Output filename."_s, u"output"_s });
    parser.addOption({ { u"c"_s, u"command"_s },
        u"Encoder command line.\n"_s
        u"Specify an encoding command line incorporating environment variables: MEDIAFX_FRAMESIZE, MEDIAFX_FRAMERATE, MEDIAFX_VIDEOFD, MEDIAFX_AUDIOFD, MEDIAFX_OUTPUT.\n"_s
        u"Or a preset: ffplay, ffmpeg, ffmpeg:lossless, ffmpeg:<args>"_s,
        u"command"_s });
    parser.addOption({ { u"p"_s, u"showPresets"_s }, u"Show command presets."_s });
    parser.addOption({ { u"w"_s, u"exitOnWarning"_s }, u"Exit on QML warnings."_s });
    parser.addPositionalArgument(u"source"_s, u"QML source URL."_s);

    parser.process(app);

    if (parser.isSet(u"showPresets"_s)) {
        fputs(
            qPrintable(u"Command presets:\n%1%2%3%4"_s.arg(
                u"\nffplay\n  %1\n"_s.arg(ffplayPreset),
                u"\nffmpeg\n  %1\n"_s.arg(ffmpegPreset),
                u"\nffmpeg:lossless\n  %1\n"_s.arg(ffmpegLosslessPreset),
                u"\nffmpeg:<args>\n  %1\n"_s.arg(ffmpegCustomPreset.arg(u"<custom ffmpeg encoding args>"_s)))),
            stdout);
        return 0;
    }

    Encoder::FrameRate frameRate = Encoder::FrameRate::parse(parser.value(u"fps"_s));
    if (frameRate.isEmpty())
        parser.showHelp(1);

    Encoder::FrameSize frameSize = Encoder::FrameSize::parse(parser.value(u"size"_s));
    if (frameSize.isEmpty())
        parser.showHelp(1);

    QString command;
    if (parser.isSet(u"command"_s)) {
        command = parser.value(u"command"_s);
        if (command == u"ffplay"_s) {
            // XXX need to handle audio eventually
            command = ffplayPreset;
        } else if (command == u"ffmpeg"_s || command.startsWith(u"ffmpeg:"_s)) {
            if (!parser.isSet(u"output"_s)) {
                parser.showHelp(1);
            }
            if (command == u"ffmpeg:lossless"_s) {
                command = ffmpegLosslessPreset;
            } else if (command == u"ffmpeg"_s) {
                command = ffmpegPreset;
            } else {
                // Treat arguments as encoding parameters
                command = ffmpegCustomPreset.arg(command.last(command.size() - u"ffmpeg:"_s.size()));
            }
        }
    }
    QString output;
    if (parser.isSet(u"output"_s)) {
        output = parser.value(u"output"_s);
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
    Session session(&encoder, parser.isSet(u"exitOnWarning"_s));
    if (!session.initialize(url)) {
        qCritical("Failed to initialize session");
        return 1;
    }
    return app.exec();
}
