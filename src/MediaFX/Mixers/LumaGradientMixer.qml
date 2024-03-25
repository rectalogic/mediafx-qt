// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Shapes
import MediaFX

/*!
    \qmltype LumaGradientMixer
    \inherits LumaMixer
    \inqmlmodule MediaFX.Mixers
    \brief LumaMixer that uses a ShapeGradient as the luma map.
*/
LumaMixer {
    id: root

    /*!
        \qmlproperty ShapeGradient ShapePath::fillGradient
        \sa ShapePath::fillGradient
    */
    property alias fillGradient: path.fillGradient

    Shape {
        parent: root.parent
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
