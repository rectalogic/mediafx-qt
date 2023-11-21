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

#include <QJSEngine>
#include <QList>
#include <QObject>
#include <QtAssert>
#include <QtQml>
#include <QtQmlIntegration>
class Clip;
class Session;
struct Interval;

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

    static MediaFX* singletonInstance();

    Session* session() { return m_session; };

    void registerClip(Clip* clip);
    void unregisterClip(Clip* clip);

    bool renderVideoFrame(const Interval& frameTimeRange);

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

        // Explicitly specify C++ ownership so that the engine doesn't delete
        // the instance.
        QJSEngine::setObjectOwnership(s_singletonInstance, QJSEngine::CppOwnership);
        return s_singletonInstance;
    }

private:
    inline static QJSEngine* s_engine = nullptr;
};