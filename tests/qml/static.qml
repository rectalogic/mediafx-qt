// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

Rectangle {
    color: "red"

    Component.onCompleted: RenderSession.endSession()
}
