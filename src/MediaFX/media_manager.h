// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interval.h"
#include <QJSEngine>
#include <QList>
#include <QObject>
#include <QQuickView> // IWYU pragma: keep
#include <QtAssert>
#include <QtQml>
#include <QtQmlIntegration>
#include <QtTypes>
#include <chrono>
class MediaClip;
using namespace std::chrono;

class MediaManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickView* window READ window FINAL)

public:
    using QObject::QObject;
    MediaManager(const microseconds& frameDuration, QQuickView* quickView, QObject* parent = nullptr);

    static MediaManager* singletonInstance();

    Q_INVOKABLE Interval createInterval(qint64 start, qint64 end) const
    {
        return Interval(start, end);
    };

    QQuickView* window() const { return m_quickView; };
    const microseconds& frameDuration() { return m_frameDuration; };

    void registerClip(MediaClip* clip);
    void unregisterClip(MediaClip* clip);

    void render();

    bool isFinishedEncoding() const { return finishedEncoding; }

signals:
    void finishEncoding();
    void frameRendered();

private:
    microseconds m_frameDuration;
    QQuickView* m_quickView;
    QList<MediaClip*> activeClips;
    bool finishedEncoding = false;
};

// https://doc.qt.io/qt-6/qqmlengine.html#QML_SINGLETON
struct MediaManagerForeign {
    Q_GADGET
    QML_FOREIGN(MediaManager)
    QML_SINGLETON
    QML_NAMED_ELEMENT(MediaManager)
public:
    inline static MediaManager* s_singletonInstance = nullptr;

    static MediaManager* create(QQmlEngine*, QJSEngine* engine)
    {
        // The instance has to exist before it is used. We cannot replace it.
        Q_ASSERT(s_singletonInstance);

        // The engine has to have the same thread affinity as the singleton.
        Q_ASSERT(engine->thread() == s_singletonInstance->thread());

        // There can only be one engine accessing the singleton.
        if (s_engine)
            Q_ASSERT(engine == s_engine);
        else
            s_engine = engine;

        // Explicitly specify C++ ownership so that the engine doesn't delete
        // the instance.
        QJSEngine::setObjectOwnership(s_singletonInstance, QJSEngine::CppOwnership);
        return s_singletonInstance;
    }

private:
    inline static QJSEngine* s_engine = nullptr;
};