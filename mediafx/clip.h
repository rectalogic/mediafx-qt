// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QMediaTimeRange>
#include <QObject>
#include <QUrl>
#include <QtQmlIntegration>
#include <QtTypes>

class Clip : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged FINAL)
    Q_PROPERTY(qint64 clipEnd READ clipEnd WRITE setClipEnd NOTIFY clipEndChanged FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Clip is an abstract base class.")

public:
    using QObject::QObject;

    Clip()
        : QObject()
        , m_clipSegment(0, -1)
        , m_currentGlobalTime(-1, -1)
        , m_nextClipTime(0, -1)
    {
    }

    QUrl url() const { return m_url; };
    void setUrl(const QUrl&);

    virtual qint64 duration() const = 0;

    qint64 clipStart() const { return m_clipSegment.start(); };
    void setClipStart(qint64);

    qint64 clipEnd() const { return m_clipSegment.end(); };
    void setClipEnd(qint64);

    bool render(const QMediaTimeRange::Interval& globalTime);

signals:
    void clipStartChanged();
    void clipEndChanged();
    void durationChanged();

protected:
    QMediaTimeRange::Interval clipTimeRange() const { return m_clipSegment; };
    void setNextClipTime(const QMediaTimeRange::Interval& time) { m_nextClipTime = time; };

    QMediaTimeRange::Interval currentGlobalTime() const { return m_currentGlobalTime; };
    void setCurrentGlobalTime(const QMediaTimeRange::Interval& currentTime) { m_currentGlobalTime = currentTime; };

    virtual void setActive(bool active) = 0;
    virtual bool active() = 0;

    virtual void loadMedia(const QUrl&) = 0;

    virtual bool renderClip(const QMediaTimeRange::Interval& globalTime) = 0;

    QMediaTimeRange::Interval nextClipTime() const { return m_nextClipTime; };

    virtual void stop();

private:
    QUrl m_url;
    QMediaTimeRange::Interval m_clipSegment;
    QMediaTimeRange::Interval m_currentGlobalTime;
    QMediaTimeRange::Interval m_nextClipTime;
};
