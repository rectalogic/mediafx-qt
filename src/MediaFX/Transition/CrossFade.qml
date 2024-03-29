// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import MediaFX

/*!
    \qmltype CrossFade
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition
    \brief Fades out the source video as it fades in the destination video.
*/
MediaTransition {
    id: root

    TransitionShaderEffect {
        source: root.source
        dest: root.dest
        time: root.time
        fragmentShader: "qrc:/shaders/crossfade.frag.qsb"
        anchors.fill: parent
    }
}
