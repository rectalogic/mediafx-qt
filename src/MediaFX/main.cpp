// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "encoder.h"
#include "session.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QMessageLogContext>
#include <QSize>
#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include <QUrl>
#include <array>
extern "C" {
#include <libavutil/log.h>
#include <libavutil/parseutils.h>
#include <libavutil/rational.h>
}
#ifdef EVENTLOGGER
#include "event_logger.h"
#endif
using namespace Qt::Literals::StringLiterals;

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
    parser.addOption({ { u"f"_s, u"fps"_s }, u"Output frames per second, can be integer or rational e.g. 30000/1001."_s, u"fps"_s, u"30"_s });
    parser.addOption({ { u"r"_s, u"sampleRate"_s }, u"Output audio sample rate (Hz)"_s, u"sampleRate"_s, u"44100"_s });
    parser.addOption({ { u"s"_s, u"size"_s }, u"Output video frame size, WxH."_s, u"size"_s, u"640x360"_s });
    parser.addOption({ { u"w"_s, u"exitOnWarning"_s }, u"Exit on QML warnings."_s });
    parser.addOption({ { u"l"_s, u"loglevel"_s }, u"FFmpeg log level."_s, u"loglevel"_s, u"info"_s });
    parser.addPositionalArgument(u"source"_s, u"QML source URL."_s);
    parser.addPositionalArgument(u"output"_s, u"Output nut video path (or '-' for stdout)."_s);

    parser.process(app);

    if (parser.isSet(u"loglevel"_s)) {
        struct LogLevel {
            QString name;
            int level;
        };
        const std::array logLevels {
            LogLevel { u"quiet"_s, AV_LOG_QUIET },
            LogLevel { u"panic"_s, AV_LOG_PANIC },
            LogLevel { u"fatal"_s, AV_LOG_FATAL },
            LogLevel { u"error"_s, AV_LOG_ERROR },
            LogLevel { u"warning"_s, AV_LOG_WARNING },
            LogLevel { u"info"_s, AV_LOG_INFO },
            LogLevel { u"verbose"_s, AV_LOG_VERBOSE },
            LogLevel { u"debug"_s, AV_LOG_DEBUG },
            LogLevel { u"trace"_s, AV_LOG_TRACE },
        };

        QString logLevel(parser.value(u"loglevel"_s));
        int level = -1;
        for (const auto& ll : logLevels) {
            if (ll.name == logLevel) {
                level = ll.level;
                break;
            }
        }
        if (level != -1)
            av_log_set_level(level);
        else {
            QStringList levelsList;
            for (const auto& ll : logLevels)
                levelsList << ll.name;
            QString levels = levelsList.join(", ");
            qCritical() << u"Invalid loglevel, must be one of "_s % levels;
            parser.showHelp(1);
        }
    }

    AVRational frameRate { 0 };
    if (av_parse_video_rate(&frameRate, qUtf8Printable(parser.value(u"fps"_s))) < 0)
        parser.showHelp(1);
    int width = 0, height = 0;
    if (av_parse_video_size(&width, &height, qUtf8Printable(parser.value(u"size"_s))) < 0)
        parser.showHelp(1);
    QSize frameSize(width, height);

    int sampleRate = parser.value(u"sampleRate"_s).toInt();

    const QStringList args = parser.positionalArguments();
    if (args.size() != 2)
        parser.showHelp(1);

    QUrl url(args.at(0));
    QString output(args.at(1));
    if (output == u"-"_s)
        output = u"pipe:"_s;

    Encoder encoder(output, frameSize, frameRate, sampleRate);
    if (!encoder.isValid()) {
        qCritical("Failed to initialize encoder");
        return 1;
    }
    Session session(&encoder, url, parser.isSet(u"exitOnWarning"_s));
    if (!session.isValid()) {
        qCritical("Failed to initialize session");
        return 1;
    }
    return app.exec();
}
