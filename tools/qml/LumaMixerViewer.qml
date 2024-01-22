// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import MediaFX
import MediaFX.Mixers
import MediaFX.Viewer

ColumnLayout {
    id: layout

    Component.onCompleted: MediaManager.window.color = MediaManager.window.palette.window

    MediaMixerViewer {
        mixer: linearMixer
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            anchors.fill: parent

            LabeledSlider {
                id: x1Slider

                label: "x1"
                to: linearMixer.width
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: y1Slider

                label: "y1"
                to: linearMixer.height
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: x2Slider

                label: "x2"
                to: linearMixer.width
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: y2Slider

                label: "y2"
                to: linearMixer.height
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: tw1Slider

                label: "transitionWidth"
                to: linearMixer.width
                Layout.fillWidth: true
            }
            LumaGradientMixer {
                id: linearMixer

                Layout.fillHeight: true
                Layout.fillWidth: true
                transitionWidth: tw1Slider.value
                fillGradient: LinearGradient {
                    x1: x1Slider.value
                    y1: y1Slider.value
                    x2: x2Slider.value
                    y2: y2Slider.value

                    GradientStop {
                        position: 0.0
                        color: "white"
                    }
                    GradientStop {
                        position: 1.0
                        color: "black"
                    }
                }
            }
        }
    }
    MediaMixerViewer {
        mixer: conicalMixer
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            anchors.fill: parent

            LabeledSlider {
                id: angleSlider

                label: "angle"
                to: 360
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: centerXSlider

                label: "centerX"
                to: conicalMixer.width
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: centerYSlider

                label: "centerY"
                to: conicalMixer.height
                Layout.fillWidth: true
            }
            LabeledSlider {
                id: tw2Slider

                label: "transitionWidth"
                to: conicalMixer.width
                Layout.fillWidth: true
            }
            LumaGradientMixer {
                id: conicalMixer

                transitionWidth: tw2Slider.value

                fillGradient: ConicalGradient {
                    angle: angleSlider.value
                    centerX: centerXSlider.value
                    centerY: centerYSlider.value

                    GradientStop {
                        position: 0.0
                        color: "white"
                    }
                    GradientStop {
                        position: 1.0
                        color: "black"
                    }
                }
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    component LabeledSlider: RowLayout {
        property alias label: label.text
        property alias from: slider.from
        property alias to: slider.to
        property alias value: slider.value

        Label {
            id: label
        }
        Slider {
            id: slider

            Layout.fillWidth: true
            ToolTip.delay: 1000
            ToolTip.text: slider.value
            ToolTip.visible: hovered
            hoverEnabled: true
        }
    }
}
