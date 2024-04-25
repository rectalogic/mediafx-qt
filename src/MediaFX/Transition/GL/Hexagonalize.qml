// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Hexagonalize
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/hexagonalize} {Hexagonalize}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real steps: 50
    /*!  */
    property real horizontalHexagons: 20

    TransitionShaderEffect {
        property alias steps: root.steps
        property alias horizontalHexagons: root.horizontalHexagons

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/hexagonalize.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIIntSpinBox {
                label: "steps"
                value: 50
                Component.onCompleted: root.steps = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "horizontalHexagons"
                initialValue: 20
                Component.onCompleted: root.horizontalHexagons = Qt.binding(() => value)
            }
        }
    }
}
