// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import QtMultimedia
import MediaFX
import MediaFX.Mixers

MediaSequence {
    id: sequence

    mediaMixers: [
        CrossFadeMixer {
        },
        WipeMixer {
            direction: WipeMixer.Direction.Down
            blindsEffect: 0.05
        },
        WipeMixer {
            direction: WipeMixer.Direction.Right
            softness: 2.0
        },
        WipeMixer {
            direction: WipeMixer.Direction.Left
            blindsEffect: 0.05
        }
    ]

    Component.onCompleted: {
        sequence.mediaSequenceEnded.connect(MediaManager.finishEncoding);
    }

    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/blue-320x180-30fps-3s.mp4")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s.mp4")
    }
    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/green-320x180-15fps-3s.mp4")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-160x120.png")
    }
    MediaClip {
        source: Qt.resolvedUrl("../fixtures/assets/yellow-320x180-15fps-3s.mp4")
    }
    MediaClip {
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/edjustforyou-320x180-15fps-5.2s.mp4")
    }
}
