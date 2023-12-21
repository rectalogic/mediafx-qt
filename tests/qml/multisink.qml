import QtQuick
import QtMultimedia
import QtQuick.Layouts
import mediafx

Item {
    id: root

    MediaClip {
        id: videoClip

        clipStart: 3000
        source: Qt.resolvedUrl("../../build/assets/red-640x360-30fps-4s.nut")

        Component.onCompleted: {
            videoClip.clipEnded.connect(MediaFX.finishEncoding);
        }
    }
    MediaClip {
        id: imageClip

        clipEnd: 3000
        source: Qt.resolvedUrl("../../build/assets/red-160x120.png")
    }
    RowLayout {
        id: layout

        anchors.fill: parent
        spacing: 0

        VideoOutput {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.rowSpan: 2
            VideoSink.clip: imageClip
        }
        ColumnLayout {
            spacing: 0

            VideoOutput {
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.75
                VideoSink.clip: videoClip
            }
            VideoOutput {
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.75
                VideoSink.clip: videoClip
            }
        }
    }
}
