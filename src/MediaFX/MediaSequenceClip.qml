// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import MediaFX.Transition

/*!
    \qmltype MediaSequenceClip
    \inqmlmodule MediaFX
    \inherits MediaClip
    \brief MediaClip type that can be used with MediaSequence.
*/
MediaClip {
    /*! The \l MediaTransition to use at the end of this clip to transition to the next clip. */
    property MediaTransition endTransition
}
