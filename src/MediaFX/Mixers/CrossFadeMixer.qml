// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import MediaFX

/*!
    \qmltype CrossFadeMixer
    \inqmlmodule MediaFX.Mixers
    \brief Fades out the source video as it fades in the destination video.
*/
MediaMixer {
    fragmentShader: "qrc:/shaders/crossfade.frag.qsb"
}
