// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype DirectionalWarp
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/directionalwarp} {DirectionalWarp}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real smoothness: 0.6
    /*!  */
    property point direction: Qt.point(-1.0, 1.0)

    TransitionShaderEffect {
        property alias smoothness: root.smoothness
        property alias direction: root.direction

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/directionalwarp.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.1
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "direction.x"
                initialValue: -1.0
                Component.onCompleted: root.direction.x = Qt.binding(() => value)
                stepSize: 0.1
                from: -1.0
                to: 1.0
            }
            UIRealSpinBox {
                label: "direction.y"
                initialValue: 1.0
                Component.onCompleted: root.direction.y = Qt.binding(() => value)
                stepSize: 0.1
                from: -1.0
                to: 1.0
            }
        }
    }
}
