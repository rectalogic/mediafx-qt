// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype StereoViewer
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/StereoViewer} {StereoViewer}.
*/
T.MediaTransition {
    id: root

    /*! How much to zoom (out) for the effect ~ 0.5 - 1.0. */
    property real zoom: 0.88
    /*! Corner radius as a fraction of the image height. */
    property real corner_radius: 0.22

    TransitionShaderEffect {
        property alias zoom: root.zoom
        property alias corner_radius: root.corner_radius

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/StereoViewer.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "zoom"
                initialValue: 0.88
                Component.onCompleted: root.zoom = Qt.binding(() => value)
                stepSize: 0.1
                from: 0.5
                to: 1.0
            }
            UIRealSpinBox {
                label: "corner_radius"
                initialValue: 0.22
                Component.onCompleted: root.corner_radius = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
        }
    }
}
