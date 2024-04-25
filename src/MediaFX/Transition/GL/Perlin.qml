// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Perlin
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/perlin} {Perlin}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real scale: 4.0
    /*!  */
    property real smoothness: 0.01
    /*!  */
    property real seed: 12.9898

    TransitionShaderEffect {
        property alias scale: root.scale
        property alias smoothness: root.smoothness
        property alias seed: root.seed

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/perlin.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "scale"
                initialValue: 4.0
                Component.onCompleted: root.scale = Qt.binding(() => value)
                stepSize: 0.1
            }
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.01
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
                stepSize: 0.01
                to: 1.0
            }
            UIRealTextInput {
                label: "seed"
                initialValue: 12.9898
                Component.onCompleted: root.seed = Qt.binding(() => value)
            }
        }
    }
}
