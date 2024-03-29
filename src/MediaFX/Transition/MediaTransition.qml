// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

/*!
    \qmltype MediaTransition
    \inherits Item
    \inqmlmodule MediaFX.Transition
    \brief Renders a video transition between two \l {MediaClip}s in a \l MediaSequence.
    \sa MediaSequence
*/
Item {
    id: root

    /*! The source item, transition from \c source to \c dest */
    property Item source
    /*! The destination item, transition from \c source to \c dest */
    property Item dest
    /*! The duration of the transition in milliseconds */
    property int duration: 1000
    property real time: 0.0
    /*! Optional UI to display in viewer */
    property Component ui

    visible: false

    states: [
        State {
            name: "active"
            when: root.source && root.dest

            PropertyChanges {
                visible: false
                target: root.source
            }
            PropertyChanges {
                visible: false
                target: root.dest
            }
        }
    ]
}
