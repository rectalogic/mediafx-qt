// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype FlyEye
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/flyeye} {FlyEye}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real size: 0.4
    /*!  */
    property real zoom: 50.0
    /*!  */
    property real colorSeparation: 0.3

    TransitionShaderEffect {
        property alias size: root.size
        property alias zoom: root.zoom
        property alias colorSeparation: root.colorSeparation

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/flyeye.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "size"
                initialValue: 0.4
                stepSize: 0.1
                Component.onCompleted: root.size = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "zoom"
                initialValue: 50.0
                Component.onCompleted: root.zoom = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "colorSeparation"
                initialValue: 0.3
                stepSize: 0.1
                Component.onCompleted: root.colorSeparation = Qt.binding(() => value)
            }
        }
    }
}
