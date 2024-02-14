// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtTypes>
class ErrorInfo;
class MediaClip;
struct FFMS_Index;

class Track {
public:
    explicit Track(MediaClip* mediaClip)
        : m_mediaClip(mediaClip)
    {
    }
    Track(Track&&) = delete;
    Track(const Track&) = delete;
    Track& operator=(Track&&) = delete;
    Track& operator=(const Track&) = delete;
    virtual ~Track() { }

    virtual bool initialize(FFMS_Index* index, int trackNum, const char* sourceFile, ErrorInfo& errorInfo) = 0;
    bool isActive() const { return m_active; };
    virtual qint64 duration() const = 0;
    virtual void stop() = 0;

    MediaClip* mediaClip() const { return m_mediaClip; };

protected:
    void setActive(bool active);

private:
    bool m_active = false;
    MediaClip* m_mediaClip;
};
