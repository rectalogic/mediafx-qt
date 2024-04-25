// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype FadeGrayscale
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/fadegrayscale} {FadeGrayscale}.
*/
T.MediaTransition {
    id: root

    /*! If 0.0, the image directly turns grayscale, if 0.9, the grayscale transition phase is very important. */
    property real intensity: 0.3

    TransitionShaderEffect {
        property alias intensity: root.intensity

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/fadegrayscale.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "intensity"
                initialValue: 0.3
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.intensity = Qt.binding(() => value)
            }
        }
    }
}
