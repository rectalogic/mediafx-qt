// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype FadeColor
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/fadecolor} {FadeColor}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property color color: Qt.rgba(0.0, 0.0, 0.0, 1.0)
    /*! If 0.0, there is no black phase, if 0.9, the black phase is very important. */
    property real colorPhase: 0.4

    TransitionShaderEffect {
        property alias color: root.color
        property alias colorPhase: root.colorPhase

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/fadecolor.frag.qsb"
    }

    ui: Component {
        Column {
            spacing: 2
            UIColorDialog {
                label: "color"
                color: Qt.rgba(0.0, 0.0, 0.0, 1.0)
                Component.onCompleted: root.color = Qt.binding(() => color)
            }
            UIRealSpinBox {
                label: "colorPhase"
                initialValue: 0.4
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.colorPhase = Qt.binding(() => value)
            }
        }
    }
}
