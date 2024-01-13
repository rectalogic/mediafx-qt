// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import QtMultimedia

State {
    id: root

    default required property MultiEffect effect
    required property VideoOutput videoOutput

    Component.onCompleted: root.effect.visible = false

    ParentChange {
        parent: root.videoOutput.parent
        target: root.effect
    }
    PropertyChanges {
        x: root.videoOutput.x
        y: root.videoOutput.y
        width: root.videoOutput.width
        height: root.videoOutput.height
        source: root.videoOutput
        visible: true
        target: root.effect
    }
    PropertyChanges {
        visible: false
        target: root.videoOutput
    }
}
