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

#include "visual_clip.h"
#include <QMediaPlayer>
#include <QObject>
#include <QQueue>
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

    bool prepareNextVideoFrame() override;

    void setActive(bool active) override;

    void stop() override;

private slots:
    void onMediaPlayerErrorOccurred(QMediaPlayer::Error error, const QString& errorString);
    void onVideoFrameChanged(const QVideoFrame& frame);

private:
    void rateControl();
    static const int MaxFrameQueueSize = 20;
    static const int MinFrameQueueSize = 5;
    QMediaPlayer mediaPlayer;
    QVideoSink mediaPlayerSink;
    QQueue<QVideoFrame> bufferedFrames;
};
