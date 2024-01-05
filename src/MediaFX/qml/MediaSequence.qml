// Copyright (C) 2024 Andrew Wason
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
import MediaFX
import "sequence.js" as Sequence

Item {
    id: root

    default required property list<MediaClip> mediaClips
    required property list<MediaMixer> mediaMixers
    property alias fillMode: video.fillMode
    property alias orientation: video.orientation

    signal mediaSequenceEnded

    Item {
        id: internal

        property int currentClipIndex: 0
        property int currentMixerIndex: 0
        property int mixStartTime

        anchors.fill: parent

        states: [
            State {
                name: "video"

                PropertyChanges {
                    Media.clip: root.mediaClips[internal.currentClipIndex]
                    target: video
                }
                PropertyChanges {
                    target: root.mediaMixers[internal.currentMixerIndex]
                    visible: false
                }
            },
            State {
                name: "mixer"

                PropertyChanges {
                    Media.clip: root.mediaClips[internal.currentClipIndex]
                    layer.enabled: true
                    visible: false
                    target: video
                }
                PropertyChanges {
                    Media.clip: (internal.currentClipIndex + 1 >= root.mediaClips.length) ? null : root.mediaClips[internal.currentClipIndex + 1]
                    layer.enabled: true
                    target: auxVideo
                }
                ParentChange {
                    parent: root
                    target: root.mediaMixers[internal.currentMixerIndex]
                }
                PropertyChanges {
                    anchors.fill: root
                    source: video
                    dest: auxVideo
                    visible: true
                    target: root.mediaMixers[internal.currentMixerIndex]
                }
            }
        ]

        Component.onCompleted: Sequence.initializeClip()

        VideoOutput {
            id: video

            anchors.fill: internal
        }
        VideoOutput {
            id: auxVideo

            fillMode: video.fillMode
            orientation: video.orientation
            anchors.fill: internal
            visible: false
        }
    }
}
