import QtQuick
import QtQuick.Layouts
import MediaFX

Item {
    id: root

    MediaClip {
        id: videoClip

        startTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-640x360-30fps-4s-rms44100.nut")

        Component.onCompleted: {
            videoClip.clipEnded.connect(RenderSession.endSession);
        }
    }
    MediaClip {
        id: imageClip

        endTime: 3000
        source: Qt.resolvedUrl("../fixtures/assets/red-160x120.png")
    }
    RowLayout {
        id: layout

        anchors.fill: parent
        spacing: 0

        VideoRenderer {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.rowSpan: 2
            mediaClip: imageClip
        }
        ColumnLayout {
            spacing: 0

            VideoRenderer {
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.75
                mediaClip: videoClip
            }
            VideoRenderer {
                Layout.fillHeight: true
                Layout.preferredWidth: root.width * 0.75
                mediaClip: videoClip
            }
        }
    }
}
