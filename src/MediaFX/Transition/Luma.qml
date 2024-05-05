// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

/*!
    \qmltype Luma
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition
    \brief Crossfade/wipe source to dest using a greyscale luma "map" image.
*/
MediaTransition {
    id: root

    /*! The item to use as the greyscale luma image. */
    default required property Item luma
    /*! Softness of the wipe, 0.0 is a hard wipe, larger values are softer. */
    property real softness: 0.0
    /*! Invert the luma. */
    property bool invert: false

    TransitionShaderEffect {
        id: shader

        property alias luma: root.luma
        property alias softness: root.softness
        readonly property real softTime: root.time * (root.softness + 1.0)
        property alias invert: root.invert

        children: shader.luma
        sourceItem: root.source
        destItem: root.dest
        time: root.time
        fragmentShader: "qrc:/shaders/luma.frag.qsb"
        anchors.fill: parent
    }
}
