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
    /*! Softness of the wip, 0.0 is a hard wipe, higher values are softer. */
    property real transitionWidth: 1.0
    readonly property real premultipliedTransitionWidth: root.time * (transitionWidth + 1.0)

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
