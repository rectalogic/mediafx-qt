// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX

/*!
    \qmltype PageCurlMixer
    \inherits MediaMixer
    \inqmlmodule MediaFX.Mixers
    \brief Transition from \l {MediaMixer::} {source} to \l {MediaMixer::} {dest} using a page curl effect.
*/
MediaMixer {
    id: root

    fragmentShader: "qrc:/shaders/pageCurl.frag.qsb"
}
