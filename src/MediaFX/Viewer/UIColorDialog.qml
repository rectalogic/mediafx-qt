// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

LabeledControl {
    id: root
    property color color

    Button {
        text: "Color"
        onClicked: colorDialog.open()
    }
    ColorDialog {
        id: colorDialog
        options: ColorDialog.ShowAlphaChannel
        selectedColor: root.color
        onAccepted: root.color = selectedColor
    }
}
