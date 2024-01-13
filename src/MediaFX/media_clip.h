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
#include <memory>
class AudioTrack;
class VideoTrack;

class MediaClip : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource REQUIRED FINAL)
    Q_PROPERTY(int startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged FINAL)
    Q_PROPERTY(int endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged FINAL)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged FINAL)
    Q_PROPERTY(Interval currentFrameTime READ currentFrameTime NOTIFY currentFrameTimeChanged FINAL)
    QML_ELEMENT

signals:
    void startTimeChanged();
    void endTimeChanged();
    void durationChanged();
    void activeChanged(bool);
    void currentFrameTimeChanged();
    void clipEnded();

public:
    using QObject::QObject;

    explicit MediaClip(QObject* parent = nullptr);
    ~MediaClip();

    QUrl source() const { return m_source; };
    void setSource(const QUrl&);

    qint64 startTime() const { return m_startTime; };
    void setStartTime(qint64 ms);

    qint64 endTime() const { return m_endTime; };
    void setEndTime(qint64 ms);

    qint64 duration() const { return m_endTime - m_startTime; };

    const Interval& currentFrameTime() const { return m_currentFrameTime; };

    void setActive(bool active);
    bool isActive() const { return m_active; };

    void render();

    void classBegin() override {};
    void componentComplete() override;

    void updateActive();

protected:
    void loadMedia();

    bool isComponentComplete() { return m_componentComplete; };

    VideoTrack* videoTrack() { return m_videoTrack.get(); };
    AudioTrack* audioTrack() { return m_audioTrack.get(); };

    friend class MediaAttached;

private:
    bool m_componentComplete = false;
    bool m_active = false;
    QUrl m_source;
    qint64 m_startTime;
    qint64 m_endTime;

    Interval m_currentFrameTime;

    std::unique_ptr<VideoTrack> m_videoTrack;
    std::unique_ptr<AudioTrack> m_audioTrack;
};
