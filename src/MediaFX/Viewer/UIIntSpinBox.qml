// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    property alias value: spinBox.value
    property alias from: spinBox.from
    property alias to: spinBox.to

    SpinBox {
        id: spinBox
        editable: true
    }
}
