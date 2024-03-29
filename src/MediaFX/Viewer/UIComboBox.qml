// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    property alias value: comboBox.currentValue
    property alias currentIndex: comboBox.currentIndex
    property alias model: comboBox.model

    ComboBox {
        id: comboBox

        textRole: "text"
        valueRole: "value"
    }
}
