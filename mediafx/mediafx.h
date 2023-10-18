// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QList>
#include <QMediaTimeRange>
#include <QObject>
#include <QtQmlIntegration>
class Clip;

class MediaFX : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    MediaFX();
    static int typeId;

    void registerClip(Clip* clip);
    void unregisterClip(Clip* clip);

    bool renderVideoFrame(const QMediaTimeRange::Interval& frameTimeRange);

private:
    QList<Clip*> activeClips;
};