// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QObject>
#include <QQmlEngine>

class MediaFX : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    // Q_PROPERTY( ... )

public:
    static int typeId;
    // members, Q_INVOKABLE functions, etc.
};