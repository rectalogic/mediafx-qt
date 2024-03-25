// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

/*!
    \qmltype MediaMixer
    \inherits ShaderEffect
    \inqmlmodule MediaFX.Mixers
    \brief Renders a video transition between two \l {MediaClip}s in a \l MediaSequence.
    \sa MediaSequence
*/
ShaderEffect {
    /*! The source item, mixer transitions from \c source to \c dest */
    property Item source
    /*! The destination item, mixer transitions from \c source to \c dest */
    property Item dest
    /*! The duration of the transition in milliseconds */
    property int duration: 1000
    property real time: 0.0

    visible: false
}
