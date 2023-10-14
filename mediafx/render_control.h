// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QObject>
#include <QQuickRenderControl>
#include <QScopedPointer>
#include <QVideoFrame>
#include <rhi/qrhi.h>
class QQuickWindow;
class QSize;

class RenderControl : public QQuickRenderControl {
    Q_OBJECT
public:
    bool install(QQuickWindow& window, QSize size);
    QVideoFrame& renderFrame();

private:
    QScopedPointer<QRhiTexture> texture;
    QScopedPointer<QRhiRenderBuffer> stencilBuffer;
    QScopedPointer<QRhiTextureRenderTarget> textureRenderTarget;
    QScopedPointer<QRhiRenderPassDescriptor> renderPassDescriptor;
    QVideoFrame videoFrame;
};
