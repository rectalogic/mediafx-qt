// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

/*!
    \qmltype LumaMixer
    \inqmlmodule MediaFX.Mixers
    \brief Crossfade/wipe source to dest using a greyscale luma "map" image.
*/
MediaMixer {
    id: root

    /*! The item to use as the greyscale luma image. */
    default required property Item luma
    /*! Softness of the wipe, 0.0 is a hard wipe, larger values are softer. */
    property real softness: 0.0
    readonly property real softTime: root.time * (root.softness + 1.0)
    /*! Invert the luma. */
    property bool invert: false

    fragmentShader: "qrc:/shaders/luma.frag.qsb"
    state: "default"

    states: State {
        name: "default"

        PropertyChanges {
            x: root.x
            y: root.y
            width: root.width
            height: root.height
            layer.enabled: true
            visible: false
            target: root.luma
        }
    }
}
