// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

Item {
    MediaClip {
        id: videoClip

        source: Qt.resolvedUrl("../fixtures/assets/blue-320x180-30fps-3s-awb44100.nut")

        Component.onCompleted: {
            videoClip.clipEnded.connect(RenderSession.endSession);
        }
    }
    VideoRenderer {
        id: videoRenderer

        anchors.fill: parent
        mediaClip: videoClip

        states: ShaderEffectState {
            name: "filter"
            videoRenderer: videoRenderer
            // From 1-2 sec into the video, switch to greyscale
            when: (videoClip.currentFrameTime.containedBy(1000, 2000))

            ShaderEffect {
                fragmentShader: "grayscale.frag.qsb"
            }
        }
    }
}
