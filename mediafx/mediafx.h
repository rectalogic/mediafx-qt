// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QJSEngine>
#include <QList>
#include <QMediaTimeRange>
#include <QObject>
#include <QtAssert>
#include <QtQml>
#include <QtQmlIntegration>
class Clip;
class Session;

class MediaFX : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;
    MediaFX(Session* session, QObject* parent = nullptr)
        : QObject(parent)
        , m_session(session)
    {
    }

    Session* session() { return m_session; };

    void registerClip(Clip* clip);
    void unregisterClip(Clip* clip);

    bool renderVideoFrame(const QMediaTimeRange::Interval& frameTimeRange);

    static int typeId;

private:
    Session* m_session;
    QList<Clip*> activeClips;
};

// https://doc.qt.io/qt-6/qqmlengine.html#QML_SINGLETON
struct MediaFXForeign {
    Q_GADGET
    QML_FOREIGN(MediaFX)
    QML_SINGLETON
    QML_NAMED_ELEMENT(MediaFX)
public:
    // Initialize this using myObject where you would previously
    // call qmlRegisterSingletonInstance().
    inline static MediaFX* s_singletonInstance = nullptr;

    static MediaFX* create(QQmlEngine*, QJSEngine* engine)
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

        MediaFX::typeId = qmlTypeId("stream.mediafx", 254, 254, "MediaFX");

        // Explicitly specify C++ ownership so that the engine doesn't delete
        // the instance.
        QJSEngine::setObjectOwnership(s_singletonInstance, QJSEngine::CppOwnership);
        return s_singletonInstance;
    }

private:
    inline static QJSEngine* s_engine = nullptr;
};