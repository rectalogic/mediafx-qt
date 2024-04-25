// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype ColorPhase
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/colorphase} {ColorPhase}.
*/
T.MediaTransition {
    id: root

    /*! Must be in [0.0, 1.0] range. all(fromStep) must be < all(toStep). */
    property vector4d fromStep: Qt.vector4d(0.0, 0.2, 0.4, 0.0)
    /*! ust be in [0.0, 1.0] range. all(fromStep) must be < all(toStep). */
    property vector4d toStep: Qt.vector4d(0.6, 0.8, 1.0, 1.0)

    TransitionShaderEffect {
        property alias fromStep: root.fromStep
        property alias toStep: root.toStep

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/colorphase.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "fromStep.x"
                initialValue: 0
                Component.onCompleted: root.fromStep.x = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "fromStep.y"
                initialValue: 0.2
                Component.onCompleted: root.fromStep.y = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "fromStep.z"
                initialValue: 0.4
                Component.onCompleted: root.fromStep.z = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "fromStep.w"
                initialValue: 0
                Component.onCompleted: root.fromStep.w = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "toStep.x"
                initialValue: 0.6
                Component.onCompleted: root.toStep.x = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "toStep.y"
                initialValue: 0.8
                Component.onCompleted: root.toStep.y = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "toStep.z"
                initialValue: 1
                Component.onCompleted: root.toStep.z = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "toStep.w"
                initialValue: 1
                Component.onCompleted: root.toStep.w = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
        }
    }
}
