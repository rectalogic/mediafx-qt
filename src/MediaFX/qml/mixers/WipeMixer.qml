// Copyright (C) 2024 Andrew Wason
//
// This file is part of mediaFX.
//
// mediaFX is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with mediaFX.
// If not, see <https://www.gnu.org/licenses/>.

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
