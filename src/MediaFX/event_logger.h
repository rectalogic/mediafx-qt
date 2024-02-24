// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDebug>
#include <QEvent>
#include <QObject>

class EventLogger : public QObject {
    Q_OBJECT
public:
    explicit EventLogger(QObject* parent = nullptr)
        : QObject(parent) {};
    EventLogger(EventLogger&&) = delete;
    EventLogger& operator=(EventLogger&&) = delete;
    ~EventLogger() override = default;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        qDebug() << obj << event->type();
        return false;
    }

private:
    Q_DISABLE_COPY(EventLogger);
};