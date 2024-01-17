// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

/*!
    \qmltype MediaMixer
    \inqmlmodule MediaFX
    \ingroup mixers
    \brief Renders a video transition between two MediaClips in a MediaSequence.
    \sa MediaSequence
*/
ShaderEffect {
    property Item source
    property Item dest
    /*! The duration of the transition in milliseconds */
    property int duration: 1000
    property real time: 0.0

    visible: false
}
