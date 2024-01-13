// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QByteArray>
#include <QObject>
#include <QQuickRenderControl>
#include <memory>
#include <rhi/qrhi.h>
class QQuickWindow;

class RenderControl : public QQuickRenderControl {
    Q_OBJECT
public:
    RenderControl(QObject* parent = nullptr)
        : QQuickRenderControl(parent) {};
    bool install(QQuickWindow* window);
    QByteArray renderVideoFrame();

private:
    std::unique_ptr<QRhiTexture> texture;
    std::unique_ptr<QRhiRenderBuffer> stencilBuffer;
    std::unique_ptr<QRhiTextureRenderTarget> textureRenderTarget;
    std::unique_ptr<QRhiRenderPassDescriptor> renderPassDescriptor;
};
