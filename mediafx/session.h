// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "render_control.h"
#include <QList>
#include <QObject>
#include <QQuickView>
class MediaFX;
class QQmlError;
class QSize;
class QUrl;

class Session : public QObject {
    Q_OBJECT

public:
    Session(QUrl& url, QSize size);

private slots:
    void quickViewStatusChanged(QQuickView::Status status);
    void engineWarnings(const QList<QQmlError>& warnings);

private:
    RenderControl renderControl;
    QQuickView quickView;
    MediaFX* mediaFX;
};