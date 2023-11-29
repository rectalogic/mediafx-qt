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

#include <QDebug>
#include <QDebugStateSaver>
#include <QtTypes>
#include <chrono>
#include <utility>
using namespace std::chrono;

struct Interval {
    constexpr Interval() noexcept = default;
    explicit constexpr Interval(const microseconds& start, const microseconds& end) noexcept
        : s(start)
        , e(end)
    {
        if (s > e) {
            std::swap(s, e);
        }
    }

    constexpr microseconds start() const noexcept { return s; }
    constexpr microseconds end() const noexcept { return e; }

    constexpr bool contains(const microseconds& time) const noexcept
    {
        return (s <= time && time < e);
    }

    constexpr Interval translated(const microseconds& offset) const
    {
        return Interval(s + offset, e + offset);
    }

    friend constexpr bool operator==(Interval lhs, Interval rhs) noexcept
    {
        return lhs.start() == rhs.start() && lhs.end() == rhs.end();
    }
    friend constexpr bool operator!=(Interval lhs, Interval rhs) noexcept
    {
        return lhs.start() != rhs.start() || lhs.end() != rhs.end();
    }

private:
    microseconds s = microseconds::zero();
    microseconds e = microseconds::zero();
};

QDebug inline operator<<(QDebug dbg, const Interval& interval)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "(" << interval.start() << ", " << interval.end() << ")";
    return dbg;
}