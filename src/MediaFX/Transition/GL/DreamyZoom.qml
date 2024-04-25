// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype DreamyZoom
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/DreamyZoom} {DreamyZoom}.
*/
T.MediaTransition {
    id: root

    /*! Rotation in degrees. */
    property real rotation: 6
    /*! Scale multiplier. */
    property real scale: 1.2

    TransitionShaderEffect {
        property alias rotation: root.rotation
        property alias scale: root.scale

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/DreamyZoom.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "rotation"
                initialValue: 6
                to: 360
                Component.onCompleted: root.rotation = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "scale"
                initialValue: 1.2
                stepSize: 0.1
                Component.onCompleted: root.scale = Qt.binding(() => value)
            }
        }
    }
}
