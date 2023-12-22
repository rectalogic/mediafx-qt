// Copyright (C) 2023 Andrew Wason
//
// This file is part of mediaFX.
//
// mediaFX is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with mediaFX.
// If not, see <https://www.gnu.org/licenses/>.

import QtQuick
import QtMultimedia
import mediafx

Item {
    MediaClip {
        id: videoClip

        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s.nut")

        Component.onCompleted: {
            // End encoding when main videoClip finishes
            videoClip.clipEnded.connect(MediaFX.finishEncoding);
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
                // 4 sec into the main video, switch to playing the ad
                when: (videoClip.clipCurrentTime.contains(4000))

                PropertyChanges {
                    Media.clip: adClip
                    target: videoOutput
                }
            }
        ]
    }
}
