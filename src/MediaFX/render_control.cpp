// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "render_control.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QQuickRenderTarget>
#include <QQuickWindow>
#include <QSize>
#include <QtCore>
#include <rhi/qrhi.h>

bool RenderControl::reconfigure()
{
#ifdef MSAA
    int sampleCount = 4;
#else
    int sampleCount = 1;
#endif

    if (!window())
        return false;
    QSize size = window()->size();
    if (texture && texture->pixelSize() == size)
        return true;

    texture.reset();
    stencilBuffer.reset();
#ifdef MSAA
    colorBuffer.reset();
#endif
    textureRenderTarget.reset();
    renderPassDescriptor.reset();

    QRhi* rhi = this->rhi();
    if (!rhi) {
        qCritical() << "No Rhi on QQuickRenderControl";
        return false;
    }

    texture.reset(rhi->newTexture(QRhiTexture::RGBA8, size, 1, QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource));
    if (!texture->create()) {
        qCritical() << "Failed to create texture";
        return false;
    }

#ifdef MSAA
    colorBuffer.reset(rhi->newRenderBuffer(QRhiRenderBuffer::Color, size, sampleCount));
    if (!colorBuffer->create()) {
        qCritical() << "Failed to create color buffer";
        return false;
    }
#endif

    // depth-stencil is mandatory with RHI, although strictly speaking the
    // scenegraph could operate without one, but it has no means to figure out
    // the lack of a ds buffer, so just be nice and provide one.
    stencilBuffer.reset(rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, size, sampleCount));
    if (!stencilBuffer->create()) {
        qCritical() << "Failed to create render buffer";
        return false;
    }

#ifdef MSAA
    QRhiColorAttachment colorAtt(colorBuffer.get());
    colorAtt.setResolveTexture(texture.get());
#else
    QRhiColorAttachment colorAtt(texture.get());
#endif
    textureRenderTarget.reset(rhi->newTextureRenderTarget({ colorAtt, stencilBuffer.get() }));
    renderPassDescriptor.reset(textureRenderTarget->newCompatibleRenderPassDescriptor());
    textureRenderTarget->setRenderPassDescriptor(renderPassDescriptor.get());
    if (!textureRenderTarget->create()) {
        qCritical() << "Failed to create render target";
        return false;
    }

    auto renderTarget = QQuickRenderTarget::fromRhiRenderTarget(textureRenderTarget.get());
    if (rhi->isYUpInFramebuffer())
        renderTarget.setMirrorVertically(true);

    // redirect Qt Quick rendering into our texture
    window()->setRenderTarget(renderTarget);

    return true;
}

QByteArray RenderControl::renderVideoFrame()
{
    if (!reconfigure())
        return QByteArray();

    QCoreApplication::processEvents();
    polishItems();
    beginFrame();
    sync();
    render();

    QRhi* rhi = this->rhi();

    QRhiReadbackResult readResult;
    QRhiResourceUpdateBatch* readbackBatch = rhi->nextResourceUpdateBatch();
    readbackBatch->readBackTexture(texture.get(), &readResult);
    this->commandBuffer()->resourceUpdate(readbackBatch);

    // offscreen frames in QRhi are synchronous, meaning the readback has been finished after endFrame()
    endFrame();

    Q_ASSERT(readResult.format == QRhiTexture::RGBA8);
    return readResult.data;
}