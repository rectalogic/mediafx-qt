// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <QMediaTimeRange>
#include <QObject>
#include <QQmlParserStatus>
#include <QUrl>
#include <QtQmlIntegration>
#include <QtTypes>

class Clip : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource REQUIRED FINAL)
    // Times are qint64 milliseconds, but stored internally as microseconds to match QVideoFrame times
    Q_PROPERTY(int clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged FINAL)
    Q_PROPERTY(int clipEnd READ clipEnd WRITE setClipEnd NOTIFY clipEndChanged FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Clip is an abstract base class.")

signals:
    void clipStartChanged();
    void clipEndChanged();

public:
    using QObject::QObject;

    Clip()
        : QObject()
        , m_clipStart(-1)
        , m_clipEnd(-1)
        , m_currentGlobalTime(-1, -1)
        , m_nextClipTime(0, -1)
    {
    }

    QUrl source() const { return m_source; };
    void setSource(const QUrl&);

    qint64 clipStart() const { return m_clipStart / 1000; };
    void setClipStart(qint64 millis);

    qint64 clipEnd() const { return m_clipEnd / 1000; };
    void setClipEnd(qint64 millis);

    bool render(const QMediaTimeRange::Interval& globalTime);

    void classBegin() override;
    void componentComplete() override;

protected:
    qint64 clipStartMicros() const { return m_clipStart; };
    void setClipStartMicros(qint64 micros);

    qint64 clipEndMicros() const { return m_clipEnd; };
    void setClipEndMicros(qint64 micros);

    QMediaTimeRange::Interval clipSegment() const { return QMediaTimeRange::Interval(m_clipStart, m_clipEnd); };

    QMediaTimeRange::Interval nextClipTime() const { return m_nextClipTime; };

    virtual void setActive(bool active);
    virtual bool active() = 0;

    virtual void loadMedia(const QUrl&) = 0;

    virtual bool renderClip(const QMediaTimeRange::Interval& globalTime) = 0;

    virtual void stop();

    bool isComponentComplete() { return m_componentComplete; };

private:
    void initializeNextClipTime();
    QMediaTimeRange::Interval currentGlobalTime() const { return m_currentGlobalTime; };
    void setCurrentGlobalTime(const QMediaTimeRange::Interval& currentTime) { m_currentGlobalTime = currentTime; };
    void setNextClipTime(const QMediaTimeRange::Interval& time) { m_nextClipTime = time; };

    bool m_componentComplete = false;
    QUrl m_source;
    qint64 m_clipStart;
    qint64 m_clipEnd;
    QMediaTimeRange::Interval m_currentGlobalTime;
    QMediaTimeRange::Interval m_nextClipTime;
};
