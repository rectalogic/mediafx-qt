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

    property Item source
    property Item dest
    property real time

    states: [
        State {
            name: "active"
            when: root.source && root.dest

            PropertyChanges {
                layer.enabled: true
                target: root.source
            }
            PropertyChanges {
                layer.enabled: true
                target: root.dest
            }
        }
    ]
}
