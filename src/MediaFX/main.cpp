// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "audio.h"
#include "encoder.h"
#include "session.h"
#include <QByteArray>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtProcessorDetection>
#include <stdio.h>
#ifdef EVENTLOGGER
#include "event_logger.h"
#endif
using namespace Qt::Literals::StringLiterals;

const auto ffmpegLoglevel = u"warning"_s;
const auto ffmpegPreamble = u" -loglevel "_s + ffmpegLoglevel + u" -hide_banner "_s
    + u"-f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -probesize 32 -thread_queue_size 32 -i async:pipe:${MEDIAFX_VIDEOFD} "_s
    + u"-f "_s + AudioFFMPEGSampleFormat + u" -ar ${MEDIAFX_SAMPLERATE} -guess_layout_max 0 -channel_layout "_s + AudioFFMPEGChannelLayout + u" -ac "_s + AudioFFMPEGChannelCount + u" -probesize 32 -thread_queue_size 32 -i async:pipe:${MEDIAFX_AUDIOFD} "_s;
// ffplay can't handle multiple input streams, so we pipe ffmpeg
const auto ffplayPreset = u"ffmpeg"_s + ffmpegPreamble + u"-codec:v copy -codec:a copy -f nut -r ${MEDIAFX_FRAMERATE} - | ffplay -hide_banner -loglevel "_s + ffmpegLoglevel + u" -autoexit -infbuf -f nut -i -"_s;
// Let ffmpeg choose codec/format based on filename suffix
const auto ffmpegPreset = u"ffmpeg"_s + ffmpegPreamble + u"-r ${MEDIAFX_FRAMERATE} -y ${MEDIAFX_OUTPUT}"_s;
const auto ffmpegLosslessPreset = u"ffmpeg"_s + ffmpegPreamble + u"-f nut -codec:v libx264 -r ${MEDIAFX_FRAMERATE} -preset veryslow -qp 0 -codec:a wavpack -y ${MEDIAFX_OUTPUT}"_s;
const auto ffmpegCustomPreset = u"ffmpeg"_s + ffmpegPreamble + u"%1 -r ${MEDIAFX_FRAMERATE} -y ${MEDIAFX_OUTPUT}"_s;

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
    // XXX add output sampleRate option for audio
    parser.addOption({ { u"f"_s, u"fps"_s }, u"Output frames per second, can be float or rational e.g. 30000/1001."_s, u"fps"_s, u"30"_s });
    parser.addOption({ { u"r"_s, u"sampleRate"_s }, u"Output audio sample rate (Hz)"_s, u"sampleRate"_s, u"44100"_s });
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

    int sampleRate = parser.value(u"sampleRate"_s).toInt();

    QString command;
    if (parser.isSet(u"command"_s)) {
        command = parser.value(u"command"_s);
        if (command == u"ffplay"_s) {
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

    Encoder encoder(frameSize, frameRate, sampleRate);
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
