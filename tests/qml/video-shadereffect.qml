// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia
import MediaFX

Item {
    MediaClip {
        id: videoClip

        source: Qt.resolvedUrl("../fixtures/assets/blue-320x180-30fps-3s.nut")

        Component.onCompleted: {
            videoClip.clipEnded.connect(MediaManager.finishEncoding);
        }
    }
    VideoOutput {
        id: videoOutput

        anchors.fill: parent
        Media.clip: videoClip

        states: ShaderEffectState {
            name: "filter"
            videoOutput: videoOutput
            // From 1-2 sec into the video, switch to greyscale
            when: (videoClip.currentFrameTime.containedBy(1000, 2000))

            ShaderEffect {
                fragmentShader: "grayscale.frag.qsb"
            }
        }
    }
}
