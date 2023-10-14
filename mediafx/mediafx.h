// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QObject>
#include <QtQmlIntegration>

class MediaFX : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    // XXX Q_PROPERTY( ... )

public:
    static int typeId;
    // XXX members, Q_INVOKABLE functions, etc.
};