// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Ripple
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/ripple} {Ripple}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real amplitude: 100.0
    /*!  */
    property real speed: 50.0

    TransitionShaderEffect {
        property alias amplitude: root.amplitude
        property alias speed: root.speed

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/ripple.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "amplitude"
                initialValue: 100.0
                Component.onCompleted: root.amplitude = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "speed"
                initialValue: 50.0
                Component.onCompleted: root.speed = Qt.binding(() => value)
            }
        }
    }
}
