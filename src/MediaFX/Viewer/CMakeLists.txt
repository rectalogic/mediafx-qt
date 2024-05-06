# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(viewer STATIC)

qt_add_qml_module(viewer
    URI MediaFX.Viewer
    QML_FILES
    TransitionViewer.qml
    LabeledControl.qml
    UICheckBox.qml
    UIIntSpinBox.qml
    UIRealSpinBox.qml
    UISlider.qml
    UIComboBox.qml
    UIRealTextInput.qml
    UIColorDialog.qml
    UIFileDialog.qml
    UITextField.qml
)