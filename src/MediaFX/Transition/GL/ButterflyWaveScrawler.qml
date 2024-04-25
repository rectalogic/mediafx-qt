// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype ButterflyWaveScrawler
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/ButterflyWaveScrawler} {ButterflyWaveScrawler}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real amplitude: 1.0
    /*!  */
    property real waves: 30
    /*!  */
    property real colorSeparation: 0.3

    TransitionShaderEffect {
        property alias amplitude: root.amplitude
        property alias waves: root.waves
        property alias colorSeparation: root.colorSeparation

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/ButterflyWaveScrawler.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "amplitude"
                initialValue: 1
                stepSize: 0.1
                Component.onCompleted: root.amplitude = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "waves"
                initialValue: 30
                to: 300
                Component.onCompleted: root.waves = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "colorSeparation"
                initialValue: 0.3
                stepSize: 0.1
                Component.onCompleted: root.colorSeparation = Qt.binding(() => value)
            }
        }
    }
}
