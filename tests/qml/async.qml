// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import MediaFX

ColumnLayout {
    spacing: 0

    AudioRenderer {
        id: audioRenderer
    }
    MediaClip {
        id: videoClipA

        endTime: 1500
        audioRenderer: audioRenderer
        source: Qt.resolvedUrl("../fixtures/assets/ednotsafe-320x180-15fps-1.53s-44100.nut")

        function resumeRendering() {
            RenderSession.resumeRendering();
        }
        onCurrentFrameTimeChanged: function () {
            if (videoClipA.currentFrameTime.contains(500)) {
                RenderSession.pauseRendering();
                Qt.callLater(resumeRendering);
            }
        }
    }
    MediaClip {
        id: videoClipB

        endTime: 1500
        audioRenderer: audioRenderer
        source: Qt.resolvedUrl("../fixtures/assets/cosmosregret-320x134par-15fps-5.3s-44100.nut")
    }
    VideoRenderer {
        mediaClip: videoClipA
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    VideoRenderer {
        mediaClip: videoClipB
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    Component.onCompleted: {
        videoClipA.clipEnded.connect(RenderSession.endSession);
    }
}
