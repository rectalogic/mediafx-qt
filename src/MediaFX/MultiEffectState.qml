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
    required property MultiEffect effect

    /*! The item to apply the effect to */
    required property Item source

    PropertyChanges {
        visible: false
        target: root.source
    }
    PropertyChanges {
        visible: true
        source: root.source
        target: root.effect
    }
}
