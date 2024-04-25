// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype CrossHatch
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/crosshatch} {CrossHatch}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property point center: Qt.point(0.5, 0.5)
    /*!  */
    property real threshold: 3.0
    /*!  */
    property real fadeEdge: 0.1

    TransitionShaderEffect {
        property alias center: root.center
        property alias threshold: root.threshold
        property alias fadeEdge: root.fadeEdge

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/crosshatch.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "center.x"
                initialValue: 0.5
                to: 1.0
                stepSize: 0.1
                Component.onCompleted: root.center.x = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "center.y"
                initialValue: 0.5
                to: 1.0
                stepSize: 0.1
                Component.onCompleted: root.center.y = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "threshold"
                initialValue: 3.0
                Component.onCompleted: root.threshold = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "fadeEdge"
                initialValue: 0.1
                stepSize: 0.1
                Component.onCompleted: root.fadeEdge = Qt.binding(() => value)
            }
        }
    }
}
