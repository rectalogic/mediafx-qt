// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtTypes>
struct FFMS_Index;
class MediaClip;
class Interval;
class ErrorInfo;

class Track {
public:
    explicit Track(MediaClip* mediaClip)
        : m_mediaClip(mediaClip)
    {
    }
    virtual ~Track() { }

    virtual bool initialize(FFMS_Index* index, const char* sourceFile, ErrorInfo& errorInfo) = 0;
    bool isActive() const { return m_active; };
    virtual qint64 duration() const = 0;
    virtual void render(const Interval& frameTime) = 0;
    virtual void stop() = 0;

    MediaClip* mediaClip() const { return m_mediaClip; };

protected:
    void setActive(bool active);

private:
    bool m_active = false;
    MediaClip* m_mediaClip;
};
