// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

RenderWindow {
    id: renderWindow
    width: RenderContext.frameSize.width
    height: RenderContext.frameSize.height
    renderSession: renderSession

    Component.onCompleted: {
        renderWindow.contentItem.enabled = false;
        renderWindow.frameReady.connect(encoder.encode);
        renderSession.renderScene.connect(renderWindow.render);
        renderSession.sessionEnded.connect(encoder.finish);
        encoder.encodingError.connect(renderSession.fatalError);
        renderSession.beginSession();
    }

    RenderSession {
        id: renderSession
        sourceUrl: RenderContext.sourceUrl
        frameRate: RenderContext.frameRate
        sampleRate: RenderContext.sampleRate
        anchors.fill: parent
    }
    Encoder {
        id: encoder
        outputFileName: RenderContext.outputFileName
        frameSize: RenderContext.frameSize
        frameRate: RenderContext.frameRate
        sampleRate: RenderContext.sampleRate
    }
}
