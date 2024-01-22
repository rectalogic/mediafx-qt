// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import QtMultimedia

/*!
    \qmltype MultiEffectState
    \inqmlmodule MediaFX
    \brief A State that applies a MultiEffect to a video when active.

    \quotefile video-multieffect.qml

    \sa State, MultiEffect
*/
State {
    id: root

    /*! The effect instance to apply to the video */
    default required property MultiEffect effect

    /*! The video to apply the effect to */
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
