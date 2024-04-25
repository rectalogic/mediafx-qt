// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    id: root

    property real initialValue: 0.0
    property real value: Number.fromLocaleString(Qt.locale(), textInput.text)

    TextInput {
        id: textInput
        text: root.initialValue
        validator: DoubleValidator {}
    }
}
