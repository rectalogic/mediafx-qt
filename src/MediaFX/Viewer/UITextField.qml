// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    id: root
    property string text

    TextField {
        id: textField
        text: root.text
        onAccepted: root.text = text
    }
}
