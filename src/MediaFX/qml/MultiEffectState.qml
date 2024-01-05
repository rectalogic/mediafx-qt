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
import QtQuick.Effects
import QtMultimedia

State {
    id: root

    default required property MultiEffect effect
    required property VideoOutput videoOutput

    Component.onCompleted: root.effect.visible = false

    ParentChange {
        parent: root.videoOutput.parent
        target: root.effect
    }
    PropertyChanges {
        x: root.videoOutput.x
        y: root.videoOutput.y
        width: root.videoOutput.width
        height: root.videoOutput.height
        source: root.videoOutput
        visible: true
        target: root.effect
    }
    PropertyChanges {
        visible: false
        target: root.videoOutput
    }
}
