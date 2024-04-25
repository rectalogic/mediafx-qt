// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition as T

T.TransitionShaderEffect {
    id: root

    property real ratio: width / height
    property real progress: root.time

    vertexShader: "qrc:/shaders/gltransition/gltransition.vert.qsb"
    textureMirroring: ShaderEffectSource.NoMirroring
    anchors.fill: parent
}
