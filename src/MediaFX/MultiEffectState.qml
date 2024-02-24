// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects

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
    required property VideoRenderer videoRenderer

    Component.onCompleted: root.effect.visible = false

    ParentChange {
        parent: root.videoRenderer.parent
        target: root.effect
    }
    PropertyChanges {
        x: root.videoRenderer.x
        y: root.videoRenderer.y
        width: root.videoRenderer.width
        height: root.videoRenderer.height
        source: root.videoRenderer
        visible: true
        target: root.effect
    }
    PropertyChanges {
        visible: false
        target: root.videoRenderer
    }
}
