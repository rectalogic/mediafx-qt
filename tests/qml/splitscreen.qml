import QtQuick
import MediaFX

Item {
    id: root

    MediaClip {
        id: videoClip

        source: Qt.resolvedUrl("../fixtures/assets/tr-subtitles-320x180-29.97fps-8s.mp4")
        audioRenderer: AudioRenderer {}

        Component.onCompleted: {
            videoClip.clipEnded.connect(root.RenderSession.session.endSession);
        }
    }

    VideoRenderer {
        id: topLeftVideo
        anchors.left: parent.left
        mediaClip: videoClip
    }
    VideoRenderer {
        id: bottomRightVideo
        anchors.right: parent.right
        anchors.top: topLeftVideo.bottom
        mediaClip: videoClip
    }
    VideoRenderer {
        id: fullVideo
        transform: [
            Translate {
                y: -fullVideo.height / 2
            },
            Scale {
                origin.x: fullVideo.width / 2
                origin.y: fullVideo.height / 2
                xScale: 0.5
                yScale: 0.5
            }
        ]
        anchors.horizontalCenter: root.horizontalCenter
        anchors.top: bottomRightVideo.bottom
        mediaClip: videoClip
    }
}
