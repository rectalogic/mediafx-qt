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

#pragma once

#include <QByteArray>
#include <QObject>
#include <QQuickRenderControl>
#include <QtCore>
#include <QtGlobal>
#include <memory>
#include <rhi/qrhi.h>
class QQuickWindow;
#if (QT_CONFIG(vulkan) && __has_include(<vulkan/vulkan.h>))
#define MEDIAFX_ENABLE_VULKAN
#endif
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QVulkanInstance>
#endif

class RenderControl : public QQuickRenderControl {
    Q_OBJECT
public:
    bool install(QQuickWindow& window);
    QByteArray renderVideoFrame();

private:
    std::unique_ptr<QRhiTexture> texture;
    std::unique_ptr<QRhiRenderBuffer> stencilBuffer;
    std::unique_ptr<QRhiTextureRenderTarget> textureRenderTarget;
    std::unique_ptr<QRhiRenderPassDescriptor> renderPassDescriptor;
#ifdef MEDIAFX_ENABLE_VULKAN
    QVulkanInstance vulkanInstance;
#endif
};
