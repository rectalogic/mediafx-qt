// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "render_control.h"
#include <QEvent>
#include <QList>
#include <QMediaTimeRange>
#include <QObject>
#include <QQuickView>
#include <QtTypes>
class MediaFX;
class QQmlError;
class QSize;
class QUrl;

class Session : public QObject {
    Q_OBJECT

public:
    Session(QSize& size, qint64 frameDuration);
    bool initialize(QUrl& url);

    qint64 frameDuration() { return m_frameDuration; };

    void render();

    bool event(QEvent* event) override;

signals:
    void exitApp(int);

private slots:
    void quickViewStatusChanged(QQuickView::Status status);
    void engineWarnings(const QList<QQmlError>& warnings);

private:
    static QEvent::Type renderEventType;
    qint64 m_frameDuration;
    QMediaTimeRange::Interval frameTime;
    RenderControl renderControl;
    QQuickView quickView;
    MediaFX* mediaFX;
};