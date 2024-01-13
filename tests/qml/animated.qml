// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

Item {
    id: container

    states: State {
        name: "reanchored"

        AnchorChanges {
            anchors.bottom: container.bottom
            anchors.right: container.right
            target: rect
        }
    }
    transitions: Transition {
        onRunningChanged: {
            if (!running)
                MediaManager.finishEncoding();
        }

        AnchorAnimation {
            duration: 2000
        }
    }

    onWidthChanged: container.state = "reanchored"

    Rectangle {
        id: rect

        width: 50
        height: 50
        color: "red"
    }
}
