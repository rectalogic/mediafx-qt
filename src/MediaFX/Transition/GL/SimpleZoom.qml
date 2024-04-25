// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype SimpleZoom
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/SimpleZoom} {SimpleZoom}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real zoom_quickness: 0.8

    TransitionShaderEffect {
        property alias zoom_quickness: root.zoom_quickness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/SimpleZoom.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "zoom_quickness"
                initialValue: 0.8
                stepSize: 0.1
                Component.onCompleted: root.zoom_quickness = Qt.binding(() => value)
            }
        }
    }
}
