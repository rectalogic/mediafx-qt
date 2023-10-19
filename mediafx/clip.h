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
    // QML doesn't support qint64, so declare these as int.
    // This is what QML MediaPlayer/QMediaPlayer does
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

    qint64 clipStart() const { return m_clipStart; };
    void setClipStart(qint64);

    qint64 clipEnd() const { return m_clipEnd; };
    void setClipEnd(qint64);

    bool render(const QMediaTimeRange::Interval& globalTime);

    void classBegin() override;
    void componentComplete() override;

protected:
    QMediaTimeRange::Interval clipSegment() const { return QMediaTimeRange::Interval(m_clipStart, m_clipEnd); };

    QMediaTimeRange::Interval nextClipTime() const { return m_nextClipTime; };
    void setNextClipTime(const QMediaTimeRange::Interval& time) { m_nextClipTime = time; };

    QMediaTimeRange::Interval currentGlobalTime() const { return m_currentGlobalTime; };
    void setCurrentGlobalTime(const QMediaTimeRange::Interval& currentTime) { m_currentGlobalTime = currentTime; };

    virtual void setActive(bool active);
    virtual bool active() = 0;

    virtual void loadMedia(const QUrl&) = 0;

    virtual bool renderClip(const QMediaTimeRange::Interval& globalTime) = 0;

    virtual void stop();

    bool isComponentComplete() { return m_componentComplete; };

private:
    bool m_componentComplete = false;
    QUrl m_source;
    qint64 m_clipStart;
    qint64 m_clipEnd;
    QMediaTimeRange::Interval m_currentGlobalTime;
    QMediaTimeRange::Interval m_nextClipTime;
};
