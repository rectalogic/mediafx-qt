// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype UndulatingBurnOut
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/undulatingBurnOut} {UndulatingBurnOut}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real smoothness: 0.03
    /*!  */
    property point center: Qt.point(0.5, 0.5)
    /*!  */
    property color color: Qt.rgba(0, 0, 0, 1)

    TransitionShaderEffect {
        property alias smoothness: root.smoothness
        property alias center: root.center
        property alias color: root.color

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/undulatingBurnOut.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.03
                stepSize: 0.01
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "center.x"
                initialValue: 0.5
                to: 1.0
                Component.onCompleted: root.center.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "center.y"
                initialValue: 0.5
                to: 1.0
                Component.onCompleted: root.center.y = Qt.binding(() => value)
            }
            UIColorDialog {
                label: "color"
                color: Qt.rgba(0, 0, 0, 1)
                Component.onCompleted: root.color = Qt.binding(() => color)
            }
        }
    }
}
