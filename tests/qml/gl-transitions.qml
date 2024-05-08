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

        Component.onCompleted: {
            sequence.mediaSequenceEnded.connect(sequence.RenderSession.session.endSession);
        }

        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Bounce {
                    objectName: "Bounce"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: Burn {
                    objectName: "Burn"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                source: Qt.resolvedUrl("../fixtures/assets/edquestions-320x180-15fps-2.4s-44100.nut")
                audioRenderer: AudioRenderer {}
                endTransition: ButterflyWaveScrawler {
                    objectName: "ButterflyWaveScrawler"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: ColorPhase {
                    objectName: "ColorPhase"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: ColourDistance {
                    objectName: "ColourDistance"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: CrazyParametricFun {
                    objectName: "CrazyParametricFun"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: CrossHatch {
                    objectName: "CrossHatch"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: CrossWarp {
                    objectName: "CrossWarp"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: CrossZoom {
                    objectName: "CrossZoom"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Cube {
                    objectName: "Cube"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: DirectionalWarp {
                    objectName: "DirectionalWarp"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: DoomScreen {
                    objectName: "DoomScreen"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Doorway {
                    objectName: "Doorway"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Dreamy {
                    objectName: "Dreamy"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: DreamyZoom {
                    objectName: "DreamyZoom"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: FadeColor {
                    objectName: "FadeColor"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: FadeGrayscale {
                    objectName: "FadeGrayscale"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: FlyEye {
                    objectName: "FlyEye"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: GlitchDisplace {
                    objectName: "GlitchDisplace"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: GlitchMemories {
                    objectName: "GlitchMemories"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: GridFlip {
                    objectName: "GridFlip"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Hexagonalize {
                    objectName: "Hexagonalize"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Kaleidoscope {
                    objectName: "Kaleidoscope"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: LinearBlur {
                    objectName: "LinearBlur"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: LuminanceMelt {
                    objectName: "LuminanceMelt"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Morph {
                    objectName: "Morph"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: Mosaic {
                    objectName: "Mosaic"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: MultiplyBlend {
                    objectName: "MultiplyBlend"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: Perlin {
                    objectName: "Perlin"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: Pixelize {
                    objectName: "Pixelize"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: PolkaDotsCurtain {
                    objectName: "PolkaDotsCurtain"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: RandomSquares {
                    objectName: "RandomSquares"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: Ripple {
                    objectName: "Ripple"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: RotateScaleFade {
                    objectName: "RotateScaleFade"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: SimpleZoom {
                    objectName: "SimpleZoom"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: SquaresWire {
                    objectName: "SquaresWire"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Squeeze {
                    objectName: "Squeeze"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: StereoViewer {
                    objectName: "StereoViewer"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: Swap {
                    objectName: "Swap"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Swirl {
                    objectName: "Swirl"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: UndulatingBurnOut {
                    objectName: "UndulatingBurnOut"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: WaterDrop {
                    objectName: "WaterDrop"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: Wind {
                    objectName: "Wind"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
                endTransition: WindowBlinds {
                    objectName: "WindowBlinds"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/lake.jpg")
                endTransition: WindowSlice {
                    objectName: "WindowSlice"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/bridge.jpg")
                endTransition: ZoomInCircles {
                    objectName: "ZoomInCircles"
                    duration: 1200
                }
            }
        }
        Component {
            MediaSequenceClip {
                endTime: 2500
                source: Qt.resolvedUrl("../fixtures/assets/road.jpg")
            }
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
