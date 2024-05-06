// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import QtMultimedia
import MediaFX
import MediaFX.Transition as T

MediaSequence {
    id: sequence

    mediaTransitions: [
        Component {
            T.CrossFade {}
        },
        Component {
            T.Wipe {
                direction: T.Wipe.Direction.Down
                blindsEffect: 0.05
            }
        },
        Component {
            T.Wipe {
                direction: T.Wipe.Direction.Right
                softness: 2.0
            }
        },
        Component {
            T.Wipe {
                direction: T.Wipe.Direction.Left
                blindsEffect: 0.05
            }
        },
        Component {
            T.PageCurl {}
        },
        Component {
            Demo3DTransition {}
        },
        Component {
            T.SamKolderWipe {}
        },
        Component {
            T.Displacement {
                displacementMapSource: Qt.resolvedUrl("../fixtures/assets/displacement.svg")
            }
        },
        Component {
            T.TextDisplacement {}
        }
    ]

    Component.onCompleted: {
        sequence.mediaSequenceEnded.connect(sequence.RenderSession.session.endSession);
    }

    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/blue-320x180-30fps-3s-awb44100.nut")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s-kal1624000.nut")
    }
    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/green-320x180-15fps-3s-kal44100.nut")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-160x120.png")
    }
    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/yellow-320x180-15fps-3s-slt16000.nut")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/edjustforyou-320x180-15fps-5.2s-44100.nut")
        audioRenderer: AudioRenderer {}
    }
    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/edquestions-320x180-15fps-2.4s-44100.nut")
        audioRenderer: AudioRenderer {}
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/cosmoswolf-320x180-15fps-4.1s-44100.nut")
        audioRenderer: AudioRenderer {}
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/ednotsafe-320x180-15fps-1.53s-44100.nut")
        audioRenderer: AudioRenderer {}
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/bbbjumprope-320x180-15fps-5.5s-44100.nut")
        audioRenderer: AudioRenderer {}
    }
}
