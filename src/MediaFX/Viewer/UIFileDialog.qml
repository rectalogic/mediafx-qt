// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

LabeledControl {
    id: root
    property url selectedFile
    property alias currentFolder: fileDialog.currentFolder
    property alias nameFilters: fileDialog.nameFilters

    Button {
        text: "Image"
        onClicked: fileDialog.open()
    }
    FileDialog {
        id: fileDialog
        onAccepted: root.selectedFile = selectedFile
    }
}
