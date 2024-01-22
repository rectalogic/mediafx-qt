// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

Item {
    MediaClip {
        id: videoClip

        startTime: 5000
        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s.mp4")

        Component.onCompleted: {
            videoClip.clipEnded.connect(MediaManager.finishEncoding);
        }
    }
    VideoRenderer {
        mediaClip: videoClip
        anchors.fill: parent
    }
}
