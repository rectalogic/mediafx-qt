// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
    property alias label: label.text

    Label {
        id: label
    }
}
