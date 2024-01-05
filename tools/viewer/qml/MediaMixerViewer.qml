// Copyright (C) 2024 Andrew Wason
//
// This file is part of mediaFX.
//
// mediaFX is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with mediaFX.
// If not, see <https://www.gnu.org/licenses/>.
import QtQuick
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Controls
import MediaFX

Item {
    id: root

    default property alias data: mixerContainer.data
    required property Item mixer

    implicitHeight: 400
    implicitWidth: 400
    state: "default"

    states: State {
        name: "default"

        PropertyChanges {
            dest: destItem
            source: sourceItem
            time: time.value
            visible: true
            target: root.mixer
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Item {
            id: mixerContainer

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Slider {
            id: time

            Layout.fillWidth: true
            from: 0
            to: 1
            value: 0.5
        }
    }
    Rectangle {
        id: sourceItem

        anchors.fill: parent
        color: "lightblue"
        layer.enabled: true
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

        anchors.fill: parent
        color: "yellow"
        layer.enabled: true
        visible: false

        Shape {
            id: qt

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
