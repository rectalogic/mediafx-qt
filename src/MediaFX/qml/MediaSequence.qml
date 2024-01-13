// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

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
