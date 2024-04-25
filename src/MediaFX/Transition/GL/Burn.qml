// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Burn
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/burn} {Burn}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property color color: Qt.rgba(0.9, 0.4, 0.2, 1.0)

    TransitionShaderEffect {
        property alias color: root.color

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/burn.frag.qsb"
    }

    ui: Component {
        Column {
            spacing: 2
            UIColorDialog {
                label: "color"
                color: Qt.rgba(0.9, 0.4, 0.2, 1.0)
                Component.onCompleted: root.color = Qt.binding(() => color)
            }
        }
    }
}
