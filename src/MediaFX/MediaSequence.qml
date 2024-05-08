// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition
import "sequence.js" as Sequence

/*!
    \qmltype MediaSequence
    \inqmlmodule MediaFX
    \inherits Item
    \brief Plays a sequence of \l MediaSequenceClip components in order, with \l MediaTransition transitions between them.

    \quotefile sequence.qml

    \sa MediaTransition
*/
Item {
    id: root

    /*! The sequence of \l MediaSequenceClip components to play in order. */
    default required property list<Component> mediaClips

    /*! The currently active MediaTransition */
    property MediaTransition currentTransition

    /*!
        \qmlproperty enumeration MediaSequence::fillMode
        \sa {VideoOutput::fillMode}
    */
    property alias fillMode: _mainVideoRenderer.fillMode

    /*!
        \qmlproperty int MediaSequence::orientation
        \sa {VideoOutput::orientation}
    */
    property alias orientation: _mainVideoRenderer.orientation

    signal mediaSequenceEnded

    Item {
        id: internal

        property MediaSequenceClip currentClip
        property MediaSequenceClip nextClip

        property int currentClipIndex: 0
        property int transitionStartTime

        anchors.fill: parent

        Component.onCompleted: Sequence.initializeClip()

        Item {
            id: _transitionContainer
            visible: false
            anchors.fill: internal
        }
        VideoRenderer {
            id: _mainVideoRenderer
            anchors.fill: internal
        }
        VideoRenderer {
            id: _auxVideoRenderer
            fillMode: _mainVideoRenderer.fillMode
            orientation: _mainVideoRenderer.orientation
            visible: false
            anchors.fill: internal
        }
    }
}
