// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype WaterDrop
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/WaterDrop} {WaterDrop}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real amplitude: 30
    /*!  */
    property real speed: 30

    TransitionShaderEffect {
        property alias amplitude: root.amplitude
        property alias speed: root.speed

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/WaterDrop.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "amplitude"
                initialValue: 30
                Component.onCompleted: root.amplitude = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "speed"
                initialValue: 30
                Component.onCompleted: root.speed = Qt.binding(() => value)
            }
        }
    }
}
