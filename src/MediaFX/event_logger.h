// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDebug>
#include <QEvent>
#include <QObject>

class EventLogger : public QObject {
    Q_OBJECT
public:
    EventLogger(QObject* parent = nullptr)
        : QObject(parent) {};

protected:
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        qDebug() << obj << event->type();
        return false;
    }
};