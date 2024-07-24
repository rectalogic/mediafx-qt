// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QByteArray>
#include <QObject>
#include <QQuickRenderControl>
#include <memory>
#include <rhi/qrhi.h>

class RenderControl : public QQuickRenderControl {
    Q_OBJECT
public:
    RenderControl(QObject* parent = nullptr)
        : QQuickRenderControl(parent) {};
    RenderControl(RenderControl&&) = delete;
    RenderControl& operator=(RenderControl&&) = delete;
    ~RenderControl() override = default;

    QByteArray renderVideoFrame();

private:
    Q_DISABLE_COPY(RenderControl);

    bool reconfigure();

    std::unique_ptr<QRhiTexture> texture;
    std::unique_ptr<QRhiRenderBuffer> stencilBuffer;
#ifdef MSAA
    std::unique_ptr<QRhiRenderBuffer> colorBuffer;
#endif
    std::unique_ptr<QRhiTextureRenderTarget> textureRenderTarget;
    std::unique_ptr<QRhiRenderPassDescriptor> renderPassDescriptor;
};
