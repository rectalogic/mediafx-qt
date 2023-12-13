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

#include "interval.h"
#include <QEvent>
#include <QList>
#include <QObject>
#include <QQuickView>
#include <QtCore>
#include <chrono>
#include <memory>
#include <qtgui-config.h>
#if (QT_CONFIG(vulkan) && __has_include(<vulkan/vulkan.h>))
#define MEDIAFX_ENABLE_VULKAN
#endif
#ifdef MEDIAFX_ENABLE_VULKAN
#include <QVulkanInstance>
#endif
class AnimationDriver;
class Encoder;
class MediaFX;
class QQmlError;
class RenderControl;
using namespace std::chrono;

class Session : public QObject {
    Q_OBJECT

public:
    Session(Encoder* encoder, QObject* parent = nullptr);
    ~Session();

    bool initialize(const QUrl& url);

    microseconds frameDuration() const noexcept { return m_frameDuration; };

    void render();

    bool event(QEvent* event) override;

signals:
    void exitApp(int);

private slots:
    void quickViewStatusChanged(QQuickView::Status status);
    void engineWarnings(const QList<QQmlError>& warnings);

private:
    static QEvent::Type renderEventType;
    Encoder* encoder;
    microseconds m_frameDuration;
    Interval frameTime;
    AnimationDriver* animationDriver;
#ifdef MEDIAFX_ENABLE_VULKAN
    QVulkanInstance vulkanInstance;
#endif
    std::unique_ptr<RenderControl> renderControl;
    std::unique_ptr<QQuickView> quickView;
    MediaFX* mediaFX;
};