// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize
    property alias value: slider.value

    Slider {
        id: slider

        Layout.fillWidth: true
        ToolTip.delay: 1000
        ToolTip.text: slider.value
        ToolTip.visible: hovered
        hoverEnabled: true
    }
}
