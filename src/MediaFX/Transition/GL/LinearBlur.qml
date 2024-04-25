// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype LinearBlur
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/LinearBlur} {LinearBlur}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real intensity: 0.1

    TransitionShaderEffect {
        property alias intensity: root.intensity

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/LinearBlur.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "intensity"
                initialValue: 0.1
                stepSize: 0.1
                Component.onCompleted: root.intensity = Qt.binding(() => value)
            }
        }
    }
}
