/*
 * Copyright (C) 2023 Andrew Wason
 *
 * This file is part of mediaFX.
 *
 * mediaFX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with mediaFX.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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

bool RenderControl::install(QQuickWindow* window)
{
    if (!initialize()) {
        qCritical() << "Failed to initialize render control";
        return false;
    }
    QSize size = window->size();

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

    // depth-stencil is mandatory with RHI, although strictly speaking the
    // scenegraph could operate without one, but it has no means to figure out
    // the lack of a ds buffer, so just be nice and provide one.
    stencilBuffer.reset(rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, size, 1));
    if (!stencilBuffer->create()) {
        qCritical() << "Failed to create render buffer";
        return false;
    }

    QRhiTextureRenderTargetDescription renderTargetDescription((QRhiColorAttachment(texture.get())));
    renderTargetDescription.setDepthStencilBuffer(stencilBuffer.get());
    textureRenderTarget.reset(rhi->newTextureRenderTarget(renderTargetDescription));
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
    window->setRenderTarget(renderTarget);

    return true;
}

QByteArray RenderControl::renderVideoFrame()
{
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