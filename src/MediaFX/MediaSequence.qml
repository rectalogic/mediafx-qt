// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition
import "sequence.js" as Sequence

/*!
    \qmltype MediaSequence
    \inqmlmodule MediaFX
    \brief Plays a sequence of \l {MediaClip}s in order, with \l MediaTransition transitions between them.

    \quotefile sequence.qml

    \sa MediaTransition
*/
Item {
    id: root

    /*! The sequence of MediaClips to play in order. */
    default required property list<MediaClip> mediaClips

    /*!
        A list of MediaTransition \l {Component}s to apply to each transition.
        If there are more clips than \a mediaTransitions, then \a mediaTransitions will be reused.
    */
    required property list<Component> mediaTransitions

    /*!
        \qmlproperty enumeration MediaSequence::fillMode
        \sa {VideoOutput::fillMode}
    */
    property alias fillMode: video.fillMode

    /*!
        \qmlproperty int MediaSequence::orientation
        \sa {VideoOutput::orientation}
    */
    property alias orientation: video.orientation

    signal mediaSequenceEnded

    Item {
        id: internal

        property int currentClipIndex: 0
        property int currentTransitionIndex: 0
        property int transitionStartTime

        anchors.fill: parent

        states: [
            State {
                name: "video"

                PropertyChanges {
                    video.mediaClip: root.mediaClips[internal.currentClipIndex]
                }
            },
            State {
                name: "transition"

                PropertyChanges {
                    video.mediaClip: root.mediaClips[internal.currentClipIndex]
                    video.visible: false
                }
                PropertyChanges {
                    auxVideo.mediaClip: (internal.currentClipIndex + 1 >= root.mediaClips.length) ? null : root.mediaClips[internal.currentClipIndex + 1]
                }
                PropertyChanges {
                    transitionLoader.visible: true
                    transitionLoader.source: video
                    transitionLoader.dest: auxVideo
                }
            }
        ]

        Loader {
            id: transitionLoader
            property Item source
            property Item dest

            visible: false
            anchors.fill: internal
            sourceComponent: root.mediaTransitions[internal.currentTransitionIndex]
            onLoaded: {
                transitionLoader.item.source = Qt.binding(() => transitionLoader.source);
                transitionLoader.item.dest = Qt.binding(() => transitionLoader.dest);
                Sequence.initializeClip();
            }
        }
        VideoRenderer {
            id: video
            anchors.fill: internal
        }
        VideoRenderer {
            id: auxVideo

            fillMode: video.fillMode
            orientation: video.orientation
            visible: false
            anchors.fill: internal
        }
    }
}
