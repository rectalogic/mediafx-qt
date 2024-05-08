// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Viewer

/*!
    \qmltype Displacement
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition
    \brief Transition using a displacement transition distortion image.
*/
MediaTransition {
    id: root

    /*! Item to use as displacement map, default is an \l Image */
    default property Item displacementMap: Image {
        anchors.fill: parent
        sourceSize.width: width
        sourceSize.height: height
        source: root.displacementMapSource
        visible: false
    }

    /*! Source url for \l displacementMap image */
    property url displacementMapSource

    /*!  */
    property real strength: 0.5

    TransitionShaderEffect {
        property alias displacementMap: root.displacementMap
        property alias strength: root.strength

        children: root.displacementMap
        sourceItem: root.source
        destItem: root.dest
        time: root.time
        fragmentShader: "qrc:/shaders/displacement.frag.qsb"
        anchors.fill: parent
    }

    ui: Component {
        Column {
            spacing: 2
            UIFileDialog {
                label: "displacementMap"
                nameFilters: ["Image files (*.jpg *.png *.svg)"]
                onSelectedFileChanged: root.displacementMapSource = selectedFile
            }
            UIRealSpinBox {
                label: "strength"
                initialValue: 0.5
                stepSize: 0.1
                Component.onCompleted: root.strength = Qt.binding(() => value)
            }
        }
    }
}
