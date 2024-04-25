// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Morph
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/morph} {Morph}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real strength: 0.1

    TransitionShaderEffect {
        property alias strength: root.strength

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/morph.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "strength"
                initialValue: 0.1
                stepSize: 0.1
                Component.onCompleted: root.strength = Qt.binding(() => value)
            }
        }
    }
}
