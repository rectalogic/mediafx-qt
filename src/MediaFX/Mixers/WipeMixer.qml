// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Shapes
import MediaFX

/*!
    \qmltype WipeMixer
    \inherits LumaGradientMixer
    \inqmlmodule MediaFX.Mixers
    \brief Wipe source to dest using a generated gradient luma map.
*/
LumaGradientMixer {
    id: root

    enum Direction {
        Up,
        Down,
        Left,
        Right
    }

    /*!
        \qmlproperty enumeration WipeMixer::direction

        Wipe direction.

        \value WipeMixer.Direction.Up up
        \value WipeMixer.Direction.Down down
        \value WipeMixer.Direction.Left left
        \value WipeMixer.Direction.Right right
    */
    property int direction: WipeMixer.Direction.Right

    /*! Split wipe into "venetian blinds" if > 0. */
    property real blindsEffect: 0.0

    function isDirectionHorizontal(): bool {
        switch (root.direction) {
        case WipeMixer.Direction.Up:
        case WipeMixer.Direction.Down:
            return false;
        case WipeMixer.Direction.Left:
        case WipeMixer.Direction.Right:
            return true;
        }
        return false;
    }

    function isDirectionForward(): bool {
        switch (root.direction) {
        case WipeMixer.Direction.Up:
        case WipeMixer.Direction.Left:
            return false;
        case WipeMixer.Direction.Down:
        case WipeMixer.Direction.Right:
            return true;
        }
        return false;
    }

    function calculatePosition(dimension: real): real {
        return root.blindsEffect > 0 ? (dimension * root.blindsEffect) : dimension;
    }

    softness: 1.0

    fillGradient: LinearGradient {
        spread: root.blindsEffect > 0 ? ShapeGradient.RepeatSpread : ShapeGradient.PadSpread
        x2: isDirectionHorizontal() ? calculatePosition(root.width) : 0
        y2: isDirectionHorizontal() ? 0 : calculatePosition(root.height)

        GradientStop {
            color: "white"
            position: isDirectionForward() ? 0.0 : 1
        }
        GradientStop {
            color: "black"
            position: isDirectionForward() ? 1.0 : 0.0
        }
    }
}
