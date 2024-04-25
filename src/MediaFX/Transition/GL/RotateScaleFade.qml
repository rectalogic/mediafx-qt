// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype RotateScaleFade
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/rotate_scale_fade} {RotateScaleFade}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property point center: Qt.point(0.5, 0.5)
    /*!  */
    property real rotations: 1
    /*!  */
    property real scale: 8
    /*!  */
    property color backColor: Qt.rgba(0.15, 0.15, 0.15, 1.0)

    TransitionShaderEffect {
        property alias center: root.center
        property alias rotations: root.rotations
        property alias scale: root.scale
        property alias backColor: root.backColor

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/rotate_scale_fade.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "center.x"
                initialValue: 0.5
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.center.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "center.y"
                initialValue: 0.5
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.center.y = Qt.binding(() => value)
            }
            UIIntSpinBox {
                label: "rotations"
                value: 1
                Component.onCompleted: root.rotations = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "scale"
                initialValue: 8
                Component.onCompleted: root.scale = Qt.binding(() => value)
            }
            UIColorDialog {
                label: "backColor"
                color: Qt.rgba(0.15, 0.15, 0.15, 1.0)
                Component.onCompleted: root.backColor = Qt.binding(() => color)
            }
        }
    }
}
