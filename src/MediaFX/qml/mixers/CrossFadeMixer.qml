// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import MediaFX

/*!
    \qmltype CrossFadeMixer
    \inqmlmodule MediaFX
    \ingroup mixers
    \brief Fades out the source video as it fades in the destination video.
*/
MediaMixer {
    fragmentShader: "qrc:/qml/mixers/crossfade.frag.qsb"
}
