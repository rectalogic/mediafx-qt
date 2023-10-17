// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "clip.h"
#include <QList>
#include <QMediaTimeRange>
#include <QObject>
#include <QVideoFrame>
#include <QtQmlIntegration>
class QVideoSink;

class VisualClip : public Clip {
    Q_OBJECT
    Q_PROPERTY(QList<QVideoSink*> videoSinks READ videoSinks WRITE setVideoSinks FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("VisualClip is an abstract base class.")

public:
    using Clip::Clip;

    QList<QVideoSink*> videoSinks() const { return m_videoSinks; };
    void setVideoSinks(const QList<QVideoSink*>&);

    bool renderVideoFrame(const QMediaTimeRange::Interval& globalTime);

protected:
    void setActive(bool active) override;

    virtual bool prepareNextVideoFrame() = 0;

    void setCurrentVideoFrame(const QVideoFrame& videoFrame) { m_currentVideoFrame = videoFrame; };

    virtual void stop() override;

private:
    QList<QVideoSink*> m_videoSinks;
    QVideoFrame m_currentVideoFrame;
};
