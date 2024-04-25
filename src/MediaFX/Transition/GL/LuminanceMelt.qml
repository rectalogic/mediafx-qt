// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype LuminanceMelt
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/luminance_melt} {LuminanceMelt}.
*/
T.MediaTransition {
    id: root

    /*! Direction of movement (false: up, true: down). */
    property bool direction: true
    /*! Luminance threshold. */
    property real l_threshold: 0.8
    /*! Does the movement takes effect above or below luminance threshold. */
    property bool above: false

    TransitionShaderEffect {
        property alias direction: root.direction
        property alias l_threshold: root.l_threshold
        property alias above: root.above

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/luminance_melt.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UICheckBox {
                label: "direction"
                value: true
                Component.onCompleted: root.direction = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "l_threshold"
                initialValue: 0.8
                Component.onCompleted: root.l_threshold = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UICheckBox {
                label: "above"
                value: false
                Component.onCompleted: root.above = Qt.binding(() => value)
            }
        }
    }
}
