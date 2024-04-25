// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Kaleidoscope
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/kaleidoscope} {Kaleidoscope}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real speed: 1
    /*!  */
    property real angle: 1
    /*!  */
    property real power: 1.5

    TransitionShaderEffect {
        property alias speed: root.speed
        property alias angle: root.angle
        property alias power: root.power

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/kaleidoscope.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "speed"
                initialValue: 1
                stepSize: 0.1
                Component.onCompleted: root.speed = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "angle"
                initialValue: 1
                stepSize: 0.1
                to: 7
                Component.onCompleted: root.angle = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "power"
                initialValue: 1.5
                stepSize: 0.1
                Component.onCompleted: root.power = Qt.binding(() => value)
            }
        }
    }
}
