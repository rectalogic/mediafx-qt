// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia
import MediaFX
import MediaFX.Transition.GL

Item {
    MediaSequence {
        id: sequence

        anchors.fill: parent

        Component.onCompleted: {
            sequence.mediaSequenceEnded.connect(sequence.RenderSession.session.endSession);
        }

        Component {
            MediaSequenceClip {
                id: clip1
                endTime: 3500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Bounce {
                    objectName: "Bounce"
                    duration: 1000
                }
                transformer: Transformer {
                    NumberAnimation on scale {
                        to: 2
                        duration: clip1.duration
                    }
                    PropertyAnimation on translate {
                        to: Qt.point(0.5, 0.5)
                        duration: clip1.duration
                    }
                }
            }
        }
        Component {
            MediaSequenceClip {
                id: clip2
                endTime: 3500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                transformer: Transformer {
                    NumberAnimation on scale {
                        to: 2
                        duration: clip2.duration
                    }
                    RotationAnimation on rotate {
                        to: 45
                        easing.type: Easing.InQuart
                        duration: clip2.duration
                    }
                }
            }
        }
    }
}
