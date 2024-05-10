// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

/*!
    \qmltype Transformer
    \inqmlmodule MediaFX
    \inherits QtObject
    \brief Computes pan, zoom, rotate transformation
*/
QtObject {
    /*! Scale factor. */
    property real scale: 1
    /*! Rotation in degrees. */
    property real rotate: 0
    /*! Translation, normalized to -1..0..1. */
    property point translate: Qt.point(0, 0)
    /*! Width of item being transformed. */
    property real width
    /*! Height of item being transformed. */
    property real height
    /*! The transformation matrix. */
    readonly property Matrix4x4 transform: Matrix4x4 {
        matrix: computeTransform(width, height, scale, rotate, translate)
    }

    function computeTransform(width: real, height: real, scale: real, rotate: real, translate: point): matrix4x4 {
        const matrix = Qt.matrix4x4();
        if (scale === 1 && rotate === 0 && translate.x === 0 && translate.y === 0)
            return matrix;
        matrix.translate(Qt.vector3d(width / 2, height / 2, 0));
        if (translate.x !== 0 || translate.y !== 0)
            matrix.translate(Qt.vector3d(-translate.x * width, -translate.y * height, 0));
        if (scale !== 1)
            matrix.scale(scale);
        if (rotate !== 0)
            matrix.rotate(rotate, Qt.vector3d(0, 0, 1));
        matrix.translate(Qt.vector3d(-width / 2, -height / 2, 0));
        return matrix;
    }
}
