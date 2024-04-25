// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype ColourDistance
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/ColourDistance} {ColourDistance}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real power: 5.0

    TransitionShaderEffect {
        property alias power: root.power

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/ColourDistance.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "power"
                initialValue: 5.0
                Component.onCompleted: root.power = Qt.binding(() => value)
            }
        }
    }
}
