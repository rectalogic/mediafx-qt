// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QList>
#include <QMediaTimeRange>
#include <QObject>
#include <QtQmlIntegration>
class VisualClip;

class MediaFX : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    MediaFX();
    static int typeId;

    void registerVisualClip(VisualClip* clip);
    void unregisterVisualClip(VisualClip* clip);

    bool renderVideoFrame(const QMediaTimeRange::Interval& frameTimeRange);

private:
    QList<VisualClip*> activeVisualClips;
};