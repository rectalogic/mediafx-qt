// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype SquaresWire
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/squareswire} {SquaresWire}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property point squares: Qt.point(10, 10)
    /*!  */
    property point direction: Qt.point(1, -0.5)
    /*!  */
    property real smoothness: 1.6

    TransitionShaderEffect {
        property alias squares: root.squares
        property alias direction: root.direction
        property alias smoothness: root.smoothness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/squareswire.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIIntSpinBox {
                label: "squares.x"
                value: 10
                Component.onCompleted: root.squares.x = Qt.binding(() => value)
            }
            UIIntSpinBox {
                label: "squares.y"
                value: 10
                Component.onCompleted: root.squares.y = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "direction.x"
                initialValue: 1
                stepSize: 0.1
                from: -1
                to: 1
                Component.onCompleted: root.direction.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "direction.y"
                initialValue: -0.5
                stepSize: 0.1
                from: -1
                to: 1
                Component.onCompleted: root.direction.y = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 1.6
                stepSize: 0.1
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
            }
        }
    }
}
