// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

Rectangle {
    id: root

    color: "red"

    Component.onCompleted: root.RenderSession.session.endSession()
}
