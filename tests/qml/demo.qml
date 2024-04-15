// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import MediaFX

Item {
    MediaClip {
        id: videoClip

        startTime: 2000
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s-kal1624000.nut")

        Component.onCompleted: {
            videoClip.clipEnded.connect(RenderSession.endSession);
        }
    }
    VideoRenderer {
        id: videoRenderer
        mediaClip: videoClip
        layer.enabled: true
        anchors.fill: parent
    }
    MultiEffect {
        source: videoRenderer
        saturation: -1.0
        anchors.fill: parent
    }
}
