// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
