// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "clip.h"
#include <QMediaPlayer>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QVideoFrame>
#include <QVideoSink>
#include <QtQmlIntegration>
#include <QtTypes>
class QUrl;

class MediaClip : public Clip {
    Q_OBJECT
    QML_ELEMENT

public:
    using Clip::Clip;
    MediaClip();

    qint64 duration() const override;

    void stop() override;

protected:
    void loadMedia(const QUrl&) override;

    bool prepareNextVideoFrame() override;

    void setActive(bool active) override;

private slots:
    void onErrorOccurred(QMediaPlayer::Error error, const QString& errorString);
    void onVideoFrameChanged(const QVideoFrame& frame);
    void onDurationChanged(qint64 duration);

private:
    void rateControl();
    static const int MaxFrameQueueSize = 20;
    static const int MinFrameQueueSize = 5;
    QMediaPlayer mediaPlayer;
    QVideoSink mediaPlayerSink;
    QQueue<QVideoFrame> frameQueue;
};
