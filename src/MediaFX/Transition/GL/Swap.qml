// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Swap
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/swap} {Swap}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real reflection: 0.4
    /*!  */
    property real perspective: 0.2
    /*!  */
    property real depth: 3

    TransitionShaderEffect {
        property alias reflection: root.reflection
        property alias perspective: root.perspective
        property alias depth: root.depth

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/swap.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "reflection"
                initialValue: 0.4
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.reflection = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "perspective"
                initialValue: 0.2
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.perspective = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "depth"
                initialValue: 3
                Component.onCompleted: root.depth = Qt.binding(() => value)
            }
        }
    }
}
