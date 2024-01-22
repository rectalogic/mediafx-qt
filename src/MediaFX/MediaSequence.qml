// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia
import MediaFX
import MediaFX.Mixers
import "sequence.js" as Sequence

/*!
    \qmltype MediaSequence
    \inqmlmodule MediaFX
    \brief Plays a sequence of MediaClips in order, with MediaMixer transitions between them.

    \quotefile sequence.qml

    \sa MediaMixer
*/
Item {
    id: root

    /*! The sequence of MediaClips to play in order. */
    default required property list<MediaClip> mediaClips
    /*!
        A list of MediaMixers to apply to each transition.
        If there are more clips than mixers, then mixers will be reused.
    */
    required property list<MediaMixer> mediaMixers
    /*!
        \qmlproperty enumeration VideoOutput::fillMode
        \sa {VideoOutput::fillMode}
    */
    property alias fillMode: video.fillMode
    /*!
        \qmlproperty int VideoOutput::orientation
        \sa {VideoOutput::orientation}
    */
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
