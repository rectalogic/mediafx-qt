// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia
import MediaFX

Item {
    MediaClip {
        id: videoClip

        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s.nut")

        Component.onCompleted: {
            // End encoding when main videoClip finishes
            videoClip.clipEnded.connect(MediaManager.finishEncoding);
        }
    }
    MediaClip {
        id: adClip

        source: Qt.resolvedUrl("../fixtures/assets/blue-320x180-30fps-3s.nut")

        // Switch back to default state when ad ends - main videoClip playing
        onClipEnded: videoOutput.state = ""
    }
    VideoOutput {
        id: videoOutput

        Media.clip: videoClip
        anchors.fill: parent

        states: [
            State {
                name: "ad"
                // 4 sec into the main video, switch to playing the ad. This also stops videoClip.currentFrameTime
                when: (videoClip.currentFrameTime.contains(4000))

                PropertyChanges {
                    Media.clip: adClip
                    target: videoOutput
                }
            }
        ]
    }
}
