// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "render_control.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageLogContext>
#include <QQuickRenderTarget>
#include <QQuickWindow>
#include <QSize>
#include <QtAssert>
#include <functional>
#include <rhi/qrhi.h>

bool RenderControl::install(QQuickWindow& window, QSize size)
{
    if (!initialize()) {
        qCritical() << "Failed to initialize render control";
        return false;
    }

    QRhi* rhi = this->rhi();

    texture.reset(rhi->newTexture(QRhiTexture::RGBA8, size, 1, QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource));
    if (!texture->create()) {
        qCritical() << "Failed to create texture";
        return false;
    }

    // depth-stencil is mandatory with RHI, although strictly speaking the
    // scenegraph could operate without one, but it has no means to figure out
    // the lack of a ds buffer, so just be nice and provide one.
    stencilBuffer.reset(rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, size, 1));
    if (!stencilBuffer->create()) {
        qCritical() << "Failed to create render buffer";
        return false;
    }

    QRhiTextureRenderTargetDescription renderTargetDescription((QRhiColorAttachment(texture.data())));
    renderTargetDescription.setDepthStencilBuffer(stencilBuffer.data());
    textureRenderTarget.reset(rhi->newTextureRenderTarget(renderTargetDescription));
    renderPassDescriptor.reset(textureRenderTarget->newCompatibleRenderPassDescriptor());
    textureRenderTarget->setRenderPassDescriptor(renderPassDescriptor.data());
    if (!textureRenderTarget->create()) {
        qCritical() << "Failed to create render target";
        return false;
    }

    auto renderTarget = QQuickRenderTarget::fromRhiRenderTarget(textureRenderTarget.data());
    if (rhi->isYUpInFramebuffer())
        renderTarget.setMirrorVertically(true);

    // redirect Qt Quick rendering into our texture
    window.setRenderTarget(renderTarget);

    return true;
}

QByteArray RenderControl::renderVideoFrame()
{
    polishItems();
    beginFrame();
    sync();
    render();

    QRhi* rhi = this->rhi();

    QByteArray frameData;
    QRhiReadbackResult readResult;
    readResult.completed = [&readResult, &rhi, &frameData] {
        Q_ASSERT(readResult.format == QRhiTexture::RGBA8);
        frameData = readResult.data;
    };
    QRhiResourceUpdateBatch* readbackBatch = rhi->nextResourceUpdateBatch();
    readbackBatch->readBackTexture(texture.data(), &readResult);
    this->commandBuffer()->resourceUpdate(readbackBatch);

    endFrame();

    // offscreen frames in QRhi are synchronous, meaning the readback has been finished at this point

    return frameData;
}