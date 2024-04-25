// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype CrazyParametricFun
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/CrazyParametricFun} {CrazyParametricFun}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real a: 4
    /*!  */
    property real b: 1
    /*!  */
    property real amplitude: 120
    /*!  */
    property real smoothness: 0.1

    TransitionShaderEffect {
        property alias a: root.a
        property alias b: root.b
        property alias amplitude: root.amplitude
        property alias smoothness: root.smoothness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/CrazyParametricFun.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "a"
                initialValue: 4
                Component.onCompleted: root.a = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "b"
                initialValue: 1
                Component.onCompleted: root.b = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "amplitude"
                initialValue: 120
                to: 300
                Component.onCompleted: root.amplitude = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.1
                stepSize: 0.1
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
            }
        }
    }
}
