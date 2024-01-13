// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

MediaMixer {
    id: root

    default required property Item luma
    property real transitionWidth: 1.0
    readonly property real premultipliedTransitionWidth: root.time * (transitionWidth + 1.0)

    fragmentShader: "qrc:/qml/mixers/luma.frag.qsb"
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
