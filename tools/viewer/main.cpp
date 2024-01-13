// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "media_manager.h"
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QList>
#include <QQuickView>
#include <QStringList>
#include <QStringLiteral>
#include <QUrl>
#include <QtLogging>
#include <chrono>
using namespace std::chrono;

int main(int argc, char* argv[])
{
    initializeMediaFX();
    QGuiApplication app(argc, argv);

    app.setOrganizationDomain(qSL("mediafx.stream"));
    app.setOrganizationName(qSL("mediaFX"));
    app.setApplicationName(qSL("mediaFX viewer"));

    QCommandLineParser parser;
    parser.setApplicationDescription(qSL("mediaFX viewer"));
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();
    parser.addPositionalArgument(qSL("source"), qSL("QML source URL."));

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        qCritical("Missing required source url");
        parser.showHelp(1);
    }
    QUrl url(args.at(0));

    QQuickView quickView;
    MediaManager* manager = new MediaManager(microseconds(33333), &quickView, &app);
    MediaManagerForeign::s_singletonInstance = manager;
    quickView.setSource(url);
    quickView.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    quickView.show();

    return app.exec();
}
