// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Wind
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/wind} {Wind}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real size: 0.2

    TransitionShaderEffect {
        property alias size: root.size

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/wind.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "size"
                initialValue: 0.2
                stepSize: 0.1
                Component.onCompleted: root.size = Qt.binding(() => value)
            }
        }
    }
}
