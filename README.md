# MediaFX

MediaFX is a video (and soon audio) non-linear media editor.
It is based on the [Qt QML](https://doc.qt.io/qt-6/qtqml-index.html) language with
[QtQuick](https://doc.qt.io/qt-6/qtquick-index.html)
and [QtMultimedia](https://doc.qt.io/qt-6/qtmultimedia-qmlmodule.html).
This means any QML effect or object can be incorporated into the generated video -
[ShaderEffect](https://doc.qt.io/qt-6/qml-qtquick-shadereffect.html),
[3D effects](https://doc.qt.io/qt-6/qtquick3d-index.html),
[Adobe After Effects animations](https://doc.qt.io/qt-6/qtlottieanimation-index.html),
[Timeline animations](https://doc.qt.io/qt-6/qtquicktimeline-index.html),
[web browser content](https://doc.qt.io/qt-6/qml-qtwebengine-webengineview.html),
and so on.

## Documentation

See the full [MediaFX documentation](https://mediafx.org/modules.html)
for the QML elements and modules.

## Dependencies

MediaFX use [Qt 6.6](https://doc.qt.io/qt-6/),
and [FFmpeg](https://ffmpeg.org/).

## Example

Given a file `demo.qml` containing:

```qml
// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import MediaFX

Item {
    MediaClip {
        id: videoClip

        startTime: 2000
        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-320x180-15fps-8s.mp4")
        audioRenderer: AudioRenderer {
        }

        Component.onCompleted: {
            videoClip.clipEnded.connect(MediaManager.finishEncoding);
        }
    }
    VideoRenderer {
        id: videoRenderer
        mediaClip: videoClip
        layer.enabled: true
        anchors.fill: parent
    }
    MultiEffect {
        source: videoRenderer
        saturation: -1.0
        anchors.fill: parent
    }
}
```

MediaClip is attached to [VideoRenderer.mediaClip](https://mediafx.org/qml-mediafx-videorenderer.html#mediaClip-prop)
and renders one second of the video frames of it's [MediaClip.source](https://mediafx.org/qml-mediafx-mediaclip.html#source-prop).
The [MultiEffect](https://doc.qt.io/qt-6/qml-qtquick-effects-multieffect.html)
filter is applied to the VideoRenderer to desaturate it.
When the clip finishes, it's [clipEnded](https://mediafx.org/qml-mediafx-mediaclip.html#clipEnded-signal) signal triggers the
[MediaManager.finishEncoding](https://mediafx.org/qml-mediafx-mediamanager.html#finishEncoding-method) slot to end encoding.

See [Qt signals and slots](https://doc.qt.io/qt-6/qtqml-syntax-signals.html#connecting-signals-to-methods-and-signals).

To run this and generate a video:
```sh-session
$ mediafx demo.qml output.nut
```
This will create a NUT format video with raw audio and video.
To encode to another format, pipe into `ffmpeg`, e.g.
```sh-session
$ mediafx demo.qml - | ffmpeg -i - output.mp4
```