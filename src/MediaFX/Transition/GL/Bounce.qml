// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Bounce
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/Bounce} {Bounce}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property color shadow_colour: Qt.rgba(0, 0, 0, 0.6)
    /*!  */
    property real shadow_height: 0.075
    /*!  */
    property real bounces: 3.0

    TransitionShaderEffect {
        property alias shadow_colour: root.shadow_colour
        property alias shadow_height: root.shadow_height
        property alias bounces: root.bounces

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/Bounce.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIColorDialog {
                label: "shadow_colour"
                color: Qt.rgba(0, 0, 0, 0.6)
                Component.onCompleted: root.shadow_colour = Qt.binding(() => color)
            }
            UIRealSpinBox {
                label: "shadow_height"
                initialValue: 0.075
                Component.onCompleted: root.shadow_height = Qt.binding(() => value)
                stepSize: 0.01
                to: 1.0
            }
            UIRealSpinBox {
                label: "bounces"
                initialValue: 3.0
                Component.onCompleted: root.bounces = Qt.binding(() => value)
            }
        }
    }
}
