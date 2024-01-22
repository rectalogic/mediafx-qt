// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia

State {
    id: root

    default required property Component effect
    required property VideoOutput videoOutput

    PropertyChanges {
        layer.effect: root.effect
        layer.enabled: true
        target: root.videoOutput
    }
}
