// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import MediaFX.Transition
import MediaFX.Viewer

/*!
    \qmltype TextDisplacement
    \inherits Displacement
    \inqmlmodule MediaFX.Transition
    \brief Transition using a text as the displacement transition distortion image.
*/
Displacement {
    id: root

    /*! Text to use as displacement map */
    property string text: "MediaFX"
    /*! Text font properties */
    property alias font: textItem.font

    Rectangle {
        color: "black"
        visible: false
        layer.enabled: true
        anchors.fill: parent

        Text {
            id: textItem
            text: root.text
            font.pixelSize: parent.height
            fontSizeMode: Text.Fit
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            anchors.fill: parent
        }
    }
    ui: Component {
        Column {
            spacing: 2
            UITextField {
                label: "text"
                text: "MediaFX"
                Component.onCompleted: root.text = Qt.binding(() => text)
            }
        }
    }
}
