// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Pixelize
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/pixelize} {Pixelize}.
*/
T.MediaTransition {
    id: root

    /*! Minimum number of squares (when the effect is at its higher level). */
    property point squaresMin: Qt.point(20, 20)
    /*! Zero disables the stepping. */
    property real steps: 50

    TransitionShaderEffect {
        property alias steps: root.steps
        property alias squaresMin: root.squaresMin

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/pixelize.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "squaresMin.x"
                initialValue: 20
                Component.onCompleted: root.squaresMin.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "squaresMin.y"
                initialValue: 20
                Component.onCompleted: root.squaresMin.y = Qt.binding(() => value)
            }
            UIIntSpinBox {
                label: "steps"
                value: 50
                Component.onCompleted: root.steps = Qt.binding(() => value)
            }
        }
    }
}
