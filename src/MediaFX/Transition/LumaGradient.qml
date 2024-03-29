// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Shapes
import MediaFX

/*!
    \qmltype LumaGradient
    \inherits Luma
    \inqmlmodule MediaFX.Transition
    \brief \l Luma that uses a ShapeGradient as the luma map.
*/
Luma {
    id: root

    /*!
        \qmlproperty ShapeGradient ShapePath::fillGradient
        \sa ShapePath::fillGradient
    */
    property alias fillGradient: path.fillGradient

    Shape {
        anchors.fill: parent
        layer.enabled: true
        visible: false

        ShapePath {
            id: path

            scale: Qt.size(root.width, root.height)

            PathPolyline {
                path: [Qt.point(0, 0), Qt.point(0, 1), Qt.point(1, 1), Qt.point(1, 0), Qt.point(0, 0)]
            }
        }
    }
}
