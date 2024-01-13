// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Shapes
import MediaFX

LumaGradientMixer {
    id: root

    enum Direction {
        Up,
        Down,
        Left,
        Right
    }

    property int direction: WipeMixer.Direction.Right
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

    transitionWidth: 1.0

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
