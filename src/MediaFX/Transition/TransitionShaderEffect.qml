// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

/*!
    \qmltype TransitionShaderEffect
    \inherits ShaderEffect
    \inqmlmodule MediaFX.Transition
    \brief \l MediaTransition subclasses that internally use a \l ShaderEffect should use a subclass of this class.
*/
ShaderEffect {
    id: root

    property alias source: sourceEffect
    property alias sourceItem: sourceEffect.sourceItem
    property alias dest: destEffect
    property alias destItem: destEffect.sourceItem
    property real time
    property int textureMirroring: ShaderEffectSource.MirrorVertically

    ShaderEffectSource {
        id: sourceEffect
        hideSource: true
        visible: false
        smooth: true
        textureMirroring: root.textureMirroring
        anchors.fill: parent
    }
    ShaderEffectSource {
        id: destEffect
        hideSource: true
        visible: false
        smooth: true
        textureMirroring: root.textureMirroring
        anchors.fill: parent
    }
}
