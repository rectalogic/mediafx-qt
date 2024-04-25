// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype WindowSlice
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/windowslice} {WindowSlice}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real count: 10.0
    /*!  */
    property real smoothness: 0.5

    TransitionShaderEffect {
        property alias count: root.count
        property alias smoothness: root.smoothness

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/windowslice.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "count"
                initialValue: 10.0
                Component.onCompleted: root.count = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "smoothness"
                initialValue: 0.5
                Component.onCompleted: root.smoothness = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
        }
    }
}
