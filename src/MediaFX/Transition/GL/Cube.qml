// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Cube
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/cube} {Cube}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real persp: 0.7
    /*!  */
    property real unzoom: 0.3
    /*!  */
    property real reflection: 0.4
    /*!  */
    property real floating: 3

    TransitionShaderEffect {
        property alias persp: root.persp
        property alias unzoom: root.unzoom
        property alias reflection: root.reflection
        property alias floating: root.floating

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/cube.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "persp"
                initialValue: 0.7
                stepSize: 0.1
                Component.onCompleted: root.persp = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "unzoom"
                initialValue: 0.3
                stepSize: 0.1
                Component.onCompleted: root.unzoom = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "reflection"
                initialValue: 0.4
                stepSize: 0.1
                Component.onCompleted: root.reflection = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "floating"
                initialValue: 3
                Component.onCompleted: root.floating = Qt.binding(() => value)
            }
        }
    }
}
