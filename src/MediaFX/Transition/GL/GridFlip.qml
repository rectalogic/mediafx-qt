// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype GridFlip
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/GridFlip} {GridFlip}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property point size: Qt.point(4, 4)
    /*!  */
    property real pause: 0.1
    /*!  */
    property real dividerWidth: 0.05
    /*!  */
    property color bgcolor: Qt.rgba(0, 0, 0, 1)
    /*!  */
    property real randomness: 0.1

    TransitionShaderEffect {
        property alias size: root.size
        property alias pause: root.pause
        property alias dividerWidth: root.dividerWidth
        property alias bgcolor: root.bgcolor
        property alias randomness: root.randomness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/GridFlip.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIIntSpinBox {
                label: "size.x"
                value: 4
                Component.onCompleted: root.size.x = Qt.binding(() => value)
            }
            UIIntSpinBox {
                label: "size.y"
                value: 4
                Component.onCompleted: root.size.y = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "pause"
                initialValue: 0.1
                Component.onCompleted: root.pause = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "dividerWidth"
                initialValue: 0.05
                stepSize: 0.01
                Component.onCompleted: root.dividerWidth = Qt.binding(() => value)
            }
            UIColorDialog {
                label: "bgcolor"
                color: Qt.rgba(0, 0, 0, 1)
                Component.onCompleted: root.bgcolor = Qt.binding(() => color)
            }
            UIRealSpinBox {
                label: "randomness"
                initialValue: 0.1
                stepSize: 0.1
                Component.onCompleted: root.randomness = Qt.binding(() => value)
            }
        }
    }
}
