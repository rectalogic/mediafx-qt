// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

State {
    id: root

    default required property Component effect
    required property VideoRenderer videoRenderer

    PropertyChanges {
        layer.effect: root.effect
        layer.enabled: true
        target: root.videoRenderer
    }
}
