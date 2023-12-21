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
