// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import QtQuick.Controls
import QtMultimedia
import MediaFX
import MediaFX.Transition.GL

Item {
    MediaSequence {
        id: sequence

        anchors.fill: parent

        mediaTransitions: [
            Component {
                Bounce {
                    objectName: "Bounce"
                    duration: 1200
                }
            },
            Component {
                Burn {
                    objectName: "Burn"
                    duration: 1200
                }
            },
            Component {
                ButterflyWaveScrawler {
                    objectName: "ButterflyWaveScrawler"
                    duration: 1200
                }
            },
            Component {
                ColorPhase {
                    objectName: "ColorPhase"
                    duration: 1200
                }
            },
            Component {
                ColourDistance {
                    objectName: "ColourDistance"
                    duration: 1200
                }
            },
            Component {
                CrazyParametricFun {
                    objectName: "CrazyParametricFun"
                    duration: 1200
                }
            },
            Component {
                CrossHatch {
                    objectName: "CrossHatch"
                    duration: 1200
                }
            },
            Component {
                CrossWarp {
                    objectName: "CrossWarp"
                    duration: 1200
                }
            },
            Component {
                CrossZoom {
                    objectName: "CrossZoom"
                    duration: 1200
                }
            },
            Component {
                Cube {
                    objectName: "Cube"
                    duration: 1200
                }
            },
            Component {
                DirectionalWarp {
                    objectName: "DirectionalWarp"
                    duration: 1200
                }
            },
            Component {
                DoomScreen {
                    objectName: "DoomScreen"
                    duration: 1200
                }
            },
            Component {
                Doorway {
                    objectName: "Doorway"
                    duration: 1200
                }
            },
            Component {
                Dreamy {
                    objectName: "Dreamy"
                    duration: 1200
                }
            },
            Component {
                DreamyZoom {
                    objectName: "DreamyZoom"
                    duration: 1200
                }
            },
            Component {
                FadeColor {
                    objectName: "FadeColor"
                    duration: 1200
                }
            },
            Component {
                FadeGrayscale {
                    objectName: "FadeGrayscale"
                    duration: 1200
                }
            },
            Component {
                FlyEye {
                    objectName: "FlyEye"
                    duration: 1200
                }
            },
            Component {
                GlitchDisplace {
                    objectName: "GlitchDisplace"
                    duration: 1200
                }
            },
            Component {
                GlitchMemories {
                    objectName: "GlitchMemories"
                    duration: 1200
                }
            },
            Component {
                GridFlip {
                    objectName: "GridFlip"
                    duration: 1200
                }
            },
            Component {
                Hexagonalize {
                    objectName: "Hexagonalize"
                    duration: 1200
                }
            },
            Component {
                Kaleidoscope {
                    objectName: "Kaleidoscope"
                    duration: 1200
                }
            },
            Component {
                LinearBlur {
                    objectName: "LinearBlur"
                    duration: 1200
                }
            },
            Component {
                LuminanceMelt {
                    objectName: "LuminanceMelt"
                    duration: 1200
                }
            },
            Component {
                Morph {
                    objectName: "Morph"
                    duration: 1200
                }
            },
            Component {
                Mosaic {
                    objectName: "Mosaic"
                    duration: 1200
                }
            },
            Component {
                MultiplyBlend {
                    objectName: "MultiplyBlend"
                    duration: 1200
                }
            },
            Component {
                Perlin {
                    objectName: "Perlin"
                    duration: 1200
                }
            },
            Component {
                Pixelize {
                    objectName: "Pixelize"
                    duration: 1200
                }
            },
            Component {
                PolkaDotsCurtain {
                    objectName: "PolkaDotsCurtain"
                    duration: 1200
                }
            },
            Component {
                RandomSquares {
                    objectName: "RandomSquares"
                    duration: 1200
                }
            },
            Component {
                Ripple {
                    objectName: "Ripple"
                    duration: 1200
                }
            },
            Component {
                RotateScaleFade {
                    objectName: "RotateScaleFade"
                    duration: 1200
                }
            },
            Component {
                SimpleZoom {
                    objectName: "SimpleZoom"
                    duration: 1200
                }
            },
            Component {
                SquaresWire {
                    objectName: "SquaresWire"
                    duration: 1200
                }
            },
            Component {
                Squeeze {
                    objectName: "Squeeze"
                    duration: 1200
                }
            },
            Component {
                StereoViewer {
                    objectName: "StereoViewer"
                    duration: 1200
                }
            },
            Component {
                Swap {
                    objectName: "Swap"
                    duration: 1200
                }
            },
            Component {
                Swirl {
                    objectName: "Swirl"
                    duration: 1200
                }
            },
            Component {
                UndulatingBurnOut {
                    objectName: "UndulatingBurnOut"
                    duration: 1200
                }
            },
            Component {
                WaterDrop {
                    objectName: "WaterDrop"
                    duration: 1200
                }
            },
            Component {
                Wind {
                    objectName: "Wind"
                    duration: 1200
                }
            },
            Component {
                WindowBlinds {
                    objectName: "WindowBlinds"
                    duration: 1200
                }
            },
            Component {
                WindowSlice {
                    objectName: "WindowSlice"
                    duration: 1200
                }
            },
            Component {
                ZoomInCircles {
                    objectName: "ZoomInCircles"
                    duration: 1200
                }
            }
        ]

        Component.onCompleted: {
            sequence.mediaSequenceEnded.connect(sequence.RenderSession.session.endSession);
        }

        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            source: Qt.resolvedUrl("../fixtures/assets/edquestions-320x180-15fps-2.4s-44100.nut")
            audioRenderer: AudioRenderer {}
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
        }
        MediaClip {
            endTime: 2500
            source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
        }
    }

    Label {
        text: sequence.currentTransition?.objectName || ""
        color: "#CCCC00"
        style: Text.Raised
        styleColor: "gray"
        font.pointSize: 36
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
