// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls

LabeledControl {
    id: root

    property alias value: spinBox.realValue
    // Can't have 2-way binding https://bugreports.qt.io/browse/QTBUG-67349
    property real initialValue: 0.0
    property alias from: spinBox.realFrom
    property alias to: spinBox.realTo
    property real stepSize: 1.0
    property alias decimals: spinBox.decimals

    SpinBox {
        id: spinBox
        editable: true
        from: decimalToInt(spinBox.realFrom)
        to: decimalToInt(spinBox.realTo)
        value: decimalToInt(root.initialValue)
        stepSize: spinBox.decimalFactor * root.stepSize

        property int decimals: 2
        property real realValue: spinBox.value / spinBox.decimalFactor
        readonly property int decimalFactor: Math.pow(10, spinBox.decimals)
        property real realFrom: 0.0
        property real realTo: 100.0

        function decimalToInt(decimal) {
            return decimal * spinBox.decimalFactor;
        }

        validator: DoubleValidator {
            bottom: Math.min(spinBox.from, spinBox.to)
            top: Math.max(spinBox.from, spinBox.to)
            decimals: spinBox.decimals
            notation: DoubleValidator.StandardNotation
        }

        textFromValue: function (value, locale) {
            return Number(value / spinBox.decimalFactor).toLocaleString(locale, 'f', spinBox.decimals);
        }

        valueFromText: function (text, locale) {
            return Math.round(Number.fromLocaleString(locale, text) * spinBox.decimalFactor);
        }
    }
}
