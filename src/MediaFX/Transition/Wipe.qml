// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Shapes
import MediaFX
import MediaFX.Viewer

/*!
    \qmltype Wipe
    \inherits LumaGradient
    \inqmlmodule MediaFX.Transition
    \brief Wipe source to dest using a generated gradient luma map.
*/
LumaGradient {
    id: root

    enum Direction {
        Up,
        Down,
        Left,
        Right
    }

    /*!
        \qmlproperty enumeration Wipe::direction

        Wipe direction.

        \value Wipe.Direction.Up up
        \value Wipe.Direction.Down down
        \value Wipe.Direction.Left left
        \value Wipe.Direction.Right right
    */
    property int direction: Wipe.Direction.Right

    /*! Split wipe into "venetian blinds" if > 0. */
    property real blindsEffect: 0.0

    function isDirectionHorizontal(): bool {
        switch (root.direction) {
        case Wipe.Direction.Up:
        case Wipe.Direction.Down:
            return false;
        case Wipe.Direction.Left:
        case Wipe.Direction.Right:
            return true;
        }
        return false;
    }

    function isDirectionForward(): bool {
        switch (root.direction) {
        case Wipe.Direction.Up:
        case Wipe.Direction.Left:
            return false;
        case Wipe.Direction.Down:
        case Wipe.Direction.Right:
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
    ui: Component {
        Column {
            spacing: 2
            UIRealSpinBox {
                label: "blindsEffect"
                Component.onCompleted: root.blindsEffect = Qt.binding(() => value)
                stepSize: 0.1
                to: 1.0
            }
            UIRealSpinBox {
                label: "softness"
                initialValue: 1.0
                Component.onCompleted: root.softness = Qt.binding(() => value)
            }
            UIComboBox {
                label: "direction"
                currentIndex: 3
                model: [
                    {
                        value: Wipe.Direction.Up,
                        text: "up"
                    },
                    {
                        value: Wipe.Direction.Down,
                        text: "down"
                    },
                    {
                        value: Wipe.Direction.Left,
                        text: "left"
                    },
                    {
                        value: Wipe.Direction.Right,
                        text: "right"
                    }
                ]
                Component.onCompleted: root.direction = Qt.binding(() => value)
            }
            UICheckBox {
                label: "invert"
                Component.onCompleted: root.invert = Qt.binding(() => value)
            }
        }
    }
}
