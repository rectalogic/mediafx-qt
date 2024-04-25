// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype PolkaDotsCurtain
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/PolkaDotsCurtain} {PolkaDotsCurtain}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real dots: 20.0
    /*!  */
    property point center: Qt.point(0, 0)

    TransitionShaderEffect {
        property alias dots: root.dots
        property alias center: root.center

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/PolkaDotsCurtain.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "dots"
                initialValue: 20.0
                Component.onCompleted: root.dots = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "center.x"
                initialValue: 0
                Component.onCompleted: root.center.x = Qt.binding(() => value)
                stepSize: 0.1
                from: -1.0
                to: 1.0
            }
            UIRealSpinBox {
                label: "center.y"
                initialValue: 0
                Component.onCompleted: root.center.y = Qt.binding(() => value)
                stepSize: 0.1
                from: -1.0
                to: 1.0
            }
        }
    }
}
