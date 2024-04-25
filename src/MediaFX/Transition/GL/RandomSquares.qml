// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype RandomSquares
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/randomsquares} {RandomSquares}.
*/
T.MediaTransition {
    id: root

    property point size: Qt.point(10, 10)
    property real smoothness: 0.5

    TransitionShaderEffect {
        property alias size: root.size
        property alias smoothness: root.smoothness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/randomsquares.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "size.x"
                initialValue: 10
                Component.onCompleted: root.size.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "size.y"
                initialValue: 10
                Component.onCompleted: root.size.y = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.5
                stepSize: 0.1
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
            }
        }
    }
}
