// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Squeeze
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/squeeze} {Squeeze}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real colorSeparation: 0.04

    TransitionShaderEffect {
        property alias colorSeparation: root.colorSeparation

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/squeeze.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "colorSeparation"
                initialValue: 0.04
                stepSize: 0.01
                to: 1
                Component.onCompleted: root.colorSeparation = Qt.binding(() => value)
            }
        }
    }
}
