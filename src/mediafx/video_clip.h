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

#include "interval.h"
#include "rate_control.h"
#include "visual_clip.h"
#include <QMediaPlayer>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVideoFrame>
#include <QVideoSink>
#include <QtQmlIntegration>
class QUrl;

class VideoClip : public VisualClip {
    Q_OBJECT
    QML_ELEMENT

public:
    using VisualClip::VisualClip;
    explicit VideoClip(QObject* parent = nullptr);

    void componentComplete() override;

protected:
    void loadMedia(const QUrl&) override;

    bool prepareNextVideoFrame(const Interval& globalTime) override;

    void onActiveChanged(bool active) override;

    void stop() override;

private slots:
    void onMediaPlayerErrorOccurred(QMediaPlayer::Error error, const QString& errorString);
    void onVideoFrameChanged(const QVideoFrame& frame);
    void onRateControl();

private:
    void play();
    QMediaPlayer mediaPlayer;
    QVideoSink mediaPlayerSink;
    QMutex mutex;
    RateControl rateControl;
    Interval lastRequestedGlobalTime;
};
