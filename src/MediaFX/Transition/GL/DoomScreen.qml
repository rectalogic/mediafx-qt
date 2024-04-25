// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype DoomScreen
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/DoomScreenTransition} {DoomScreen}.
*/
T.MediaTransition {
    id: root

    /*! Number of total bars/columns. */
    property real bars: 30
    /*! Multiplier for speed ratio. 0 = no variation when going down, higher = some elements go much faster. */
    property real amplitude: 2
    /*! Further variations in speed. 0 = no noise, 1 = super noisy (ignore frequency). */
    property real noise: 0.1
    /*! Speed variation horizontally. the bigger the value, the shorter the waves. */
    property real frequency: 0.5
    /*! How much the bars seem to "run" from the middle of the screen first (sticking to the sides). 0 = no drip, 1 = curved drip. */
    property real dripScale: 0.5

    TransitionShaderEffect {
        property alias bars: root.bars
        property alias amplitude: root.amplitude
        property alias noise: root.noise
        property alias frequency: root.frequency
        property alias dripScale: root.dripScale

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/DoomScreenTransition.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIIntSpinBox {
                label: "bars"
                value: 30
                to: 300
                Component.onCompleted: root.bars = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "amplitude"
                initialValue: 2
                stepSize: 0.1
                Component.onCompleted: root.amplitude = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "noise"
                initialValue: 0.1
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.noise = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "frequency"
                initialValue: 0.5
                stepSize: 0.1
                Component.onCompleted: root.frequency = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "dripScale"
                initialValue: 0.5
                stepSize: 0.1
                to: 1
                Component.onCompleted: root.dripScale = Qt.binding(() => value)
            }
        }
    }
}
