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
#include <QObject>
#include <QQmlParserStatus>
#include <QUrl>
#include <QtQmlIntegration>
#include <QtTypes>
#include <chrono>
using namespace std::chrono;

class Clip : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource REQUIRED FINAL)
    // Times are qint64 milliseconds (ms), but stored internally as microseconds (us) to match QVideoFrame times
    Q_PROPERTY(int clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged FINAL)
    Q_PROPERTY(int clipEnd READ clipEnd WRITE setClipEnd NOTIFY clipEndChanged FINAL)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Clip is an abstract base class.")

signals:
    void clipStartChanged();
    void clipEndChanged();
    void activeChanged(bool);
    void clipEnded();

protected slots:
    virtual void onActiveChanged(bool active) {};

public:
    using QObject::QObject;

    explicit Clip(QObject* parent = nullptr);

    QUrl source() const { return m_source; };
    void setSource(const QUrl&);

    qint64 clipStart() const { return duration_cast<milliseconds>(m_clipStart).count(); };
    void setClipStart(qint64 ms);

    qint64 clipEnd() const { return duration_cast<milliseconds>(m_clipEnd).count(); };
    void setClipEnd(qint64 ms);

    void setActive(bool active);
    bool isActive() const { return m_active; };

    bool render(const Interval& globalTime);

    void classBegin() override;
    void componentComplete() override;

protected:
    microseconds clipStart_us() const { return m_clipStart; };
    void setClipStart_us(microseconds us);

    microseconds clipEnd_us() const { return m_clipEnd; };
    void setClipEnd_us(microseconds us);

    Interval clipSegment() const { return Interval(m_clipStart, m_clipEnd); };

    Interval nextClipTime() const { return m_nextClipTime; };

    virtual void loadMedia(const QUrl&) = 0;

    virtual bool renderClip(const Interval& globalTime) = 0;

    virtual void stop();

    bool isComponentComplete() { return m_componentComplete; };

private:
    void initializeNextClipTime();
    Interval currentGlobalTime() const { return m_currentGlobalTime; };
    void setCurrentGlobalTime(const Interval& currentTime) { m_currentGlobalTime = currentTime; };
    void setNextClipTime(const Interval& time) { m_nextClipTime = time; };

    bool m_componentComplete = false;
    bool m_active = false;
    bool m_stopped = false;
    QUrl m_source;
    microseconds m_clipStart;
    microseconds m_clipEnd;
    Interval m_currentGlobalTime;
    Interval m_nextClipTime;
};
