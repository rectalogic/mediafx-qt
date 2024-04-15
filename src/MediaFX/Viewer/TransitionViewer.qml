// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Controls
import MediaFX
import MediaFX.Transition

ColumnLayout {
    id: root

    property MediaTransition mediaTransition

    Component.onCompleted: {
        // Create the transitionComponent context object set in main.cpp
        root.mediaTransition = transitionComponent.createObject(transitionContainer, {
            dest: destItem,
            source: sourceItem,
            time: Qt.binding(() => timeSlider.value),
            "anchors.fill": transitionContainer
        });
    }

    Item {
        id: transitionContainer

        implicitWidth: 640
        implicitHeight: 360
        Layout.fillWidth: true
        Layout.fillHeight: true

        Rectangle {
            id: sourceItem

            color: "lightblue"
            anchors.fill: parent
            visible: false

            Shape {
                anchors.fill: parent

                ShapePath {
                    fillColor: "orange"
                    fillRule: ShapePath.WindingFill
                    scale: Qt.size(sourceItem.width / 160, sourceItem.height / 160)
                    strokeColor: "darkgray"
                    strokeWidth: 3

                    PathPolyline {
                        path: [Qt.point(80, 0), Qt.point(130, 160), Qt.point(0, 55), Qt.point(160, 55), Qt.point(30, 160), Qt.point(80, 0),]
                    }
                }
            }
        }
        Rectangle {
            id: destItem

            color: "yellow"
            anchors.fill: parent
            visible: false

            Shape {
                anchors.fill: parent

                ShapePath {
                    fillColor: "green"
                    scale: Qt.size(destItem.width / 200, destItem.height / 200)
                    strokeColor: "darkGray"
                    strokeWidth: 3

                    PathAngleArc {
                        centerX: 100
                        centerY: 100
                        radiusX: 100
                        radiusY: 100
                        sweepAngle: 360
                    }
                }
            }
        }
    }
    Slider {
        id: timeSlider

        Layout.fillWidth: true
        from: 0
        to: 1
        value: 0.5
    }
    CheckBox {
        id: controlsCheckBox

        text: "Show Controls"
        enabled: loader.item
    }
    Button {
        text: "Dump Transition"

        TapHandler {
            acceptedModifiers: Qt.ShiftModifier
            onTapped: root.dumpItemTree()
        }
        TapHandler {
            acceptedModifiers: Qt.NoModifier
            onTapped: root.mediaTransition?.dumpItemTree()
        }
    }
    Drawer {
        edge: Qt.TopEdge
        closePolicy: Popup.NoAutoClose
        modal: false
        visible: controlsCheckBox.checked

        Loader {
            id: loader
            sourceComponent: root.mediaTransition?.ui
        }
    }
}
