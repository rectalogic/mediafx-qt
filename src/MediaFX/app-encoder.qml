// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

RenderWindow {
    id: renderWindow
    width: RenderSession.renderContext.frameSize.width
    height: RenderSession.renderContext.frameSize.height

    Component.onCompleted: {
        renderWindow.contentItem.enabled = false;
        renderWindow.frameReady.connect(encoder.encode);
        RenderSession.renderScene.connect(renderWindow.render);
        RenderSession.sessionEnded.connect(encoder.finish);
        encoder.encodingError.connect(RenderSession.fatalError);
        RenderSession.beginSession();
    }

    Loader {
        id: loader
        source: RenderSession.renderContext.sourceUrl
        anchors.fill: parent
        Component.onCompleted: {
            if (loader.status == Loader.Error)
                RenderSession.fatalError();
        }
    }

    Encoder {
        id: encoder
        outputFileName: RenderSession.renderContext.outputFileName
    }
}
