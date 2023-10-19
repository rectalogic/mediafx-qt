// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QByteArray>
#include <QObject>
#include <QQuickRenderControl>
#include <QScopedPointer>
#include <rhi/qrhi.h>
class QQuickWindow;

class RenderControl : public QQuickRenderControl {
    Q_OBJECT
public:
    bool install(QQuickWindow& window);
    QByteArray renderVideoFrame();

private:
    QScopedPointer<QRhiTexture> texture;
    QScopedPointer<QRhiRenderBuffer> stencilBuffer;
    QScopedPointer<QRhiTextureRenderTarget> textureRenderTarget;
    QScopedPointer<QRhiRenderPassDescriptor> renderPassDescriptor;
};
