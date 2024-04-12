// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

/*!
    \qmltype PageCurl
    \inherits MediaTransition
    \inqmlmodule MediaFX.Transition
    \brief Transition from \l {MediaTransition::} {source} to \l {MediaTransition::} {dest} using a page curl effect.
*/
MediaTransition {
    id: root

    TransitionShaderEffect {
        sourceItem: root.source
        destItem: root.dest
        time: root.time
        fragmentShader: "qrc:/shaders/pageCurl.frag.qsb"
        anchors.fill: parent
    }
}
