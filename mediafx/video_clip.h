// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
    VideoClip();

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
