// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import MediaFX.Viewer
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    visible: true
    title: "MediaFX Transition Viewer"

    Component.onCompleted: {
        width = viewer.implicitWidth;
        height = viewer.implicitHeight;
    }

    TransitionViewer {
        id: viewer
        anchors.fill: parent
    }
}
