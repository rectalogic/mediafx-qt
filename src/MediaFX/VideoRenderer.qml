// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia

/*!
    \qmltype VideoRenderer
    \inqmlmodule MediaFX
    \brief Renders video frames from a MediaClip.
*/
VideoOutput {
    id: root

    /*! The MediaClip to render */
    property MediaClip mediaClip

    onMediaClipChanged: {
        MediaManager.updateVideoSinks(internal.previousMediaClip, root.mediaClip, root.videoSink);
        internal.previousMediaClip = root.mediaClip;
    }

    QtObject {
        id: internal

        property MediaClip previousMediaClip
    }
}
