// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Viewer

/*!
    \qmltype SamKolderWipe
    \inherits Luma
    \inqmlmodule MediaFX.Transition
    \brief \l Luma transition based on \l {https://motionarray.com/learn/premiere-pro/sam-kolder-transition-premiere-pro/} {Sam Kolder's transition}.
*/
Luma {
    id: root

    softness: 1.0

    // Use the dest as the luma
    luma: effect.dest

    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "softness"
                initialValue: 1.0
                stepSize: 0.1
                Component.onCompleted: root.softness = Qt.binding(() => value)
            }
            UICheckBox {
                label: "invert"
                Component.onCompleted: root.invert = Qt.binding(() => value)
            }
        }
    }
}
