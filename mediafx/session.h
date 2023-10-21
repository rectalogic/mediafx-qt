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
class AnimationDriver;

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
    AnimationDriver* animationDriver;
};