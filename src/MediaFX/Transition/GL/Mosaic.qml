// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T
import MediaFX.Viewer

/*!
    \qmltype Mosaic
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition.GL
    \brief Implements gl-transitions \l {https://gl-transitions.com/editor/Mosaic} {Mosaic}.
*/
T.MediaTransition {
    id: root

    /*!  */
    property real endx: 2
    /*!  */
    property real endy: -1

    TransitionShaderEffect {
        property alias endx: root.endx
        property alias endy: root.endy

        sourceItem: root.source
        destItem: root.dest
        progress: root.time

        fragmentShader: "qrc:/shaders/gltransition/Mosaic.frag.qsb"
    }
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "endx"
                initialValue: 2
                from: -10
                Component.onCompleted: root.endx = Qt.binding(() => value)
            }
            UIRealSpinBox {
                label: "endy"
                initialValue: -1
                from: -10
                Component.onCompleted: root.endy = Qt.binding(() => value)
            }
        }
    }
}
