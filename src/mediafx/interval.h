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
#include <utility>

struct Interval {
    constexpr Interval() noexcept = default;
    explicit constexpr Interval(qint64 start, qint64 end) noexcept
        : s(start)
        , e(end)
    {
        if (s > e) {
            std::swap(s, e);
        }
    }

    constexpr qint64 start() const noexcept { return s; }
    constexpr qint64 end() const noexcept { return e; }

    constexpr bool contains(qint64 time) const noexcept
    {
        return (s <= time && time < e);
    }

    constexpr Interval translated(qint64 offset) const
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
    qint64 s = 0;
    qint64 e = 0;
};
