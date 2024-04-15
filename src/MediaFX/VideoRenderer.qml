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
        if (internal.previousMediaClip)
            internal.previousMediaClip.removeVideoSink(root.videoSink);
        if (root.mediaClip)
            root.mediaClip.addVideoSink(root.videoSink);
        internal.previousMediaClip = root.mediaClip;
    }

    QtObject {
        id: internal

        property MediaClip previousMediaClip
    }
}
