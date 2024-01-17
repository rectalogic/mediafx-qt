// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDebug>
#include <QDebugStateSaver>
#include <QObject>
#include <QtQmlIntegration>
#include <QtTypes>
#include <chrono>
#include <compare>
#include <ratio>
#include <utility>
using namespace std::chrono;

class Interval {
    Q_GADGET
    QML_VALUE_TYPE(interval)
    Q_PROPERTY(int start READ start FINAL)
    Q_PROPERTY(int end READ end FINAL)
    Q_PROPERTY(int duration READ duration FINAL)

public:
    constexpr Interval() noexcept = default;

    explicit constexpr Interval(const microseconds& start, const microseconds& end) noexcept
        : s(start)
        , e(end)
    {
        if (s > e) {
            std::swap(s, e);
        }
    }

    explicit constexpr Interval(qint64 start, qint64 end) noexcept
        : Interval(milliseconds(start), milliseconds(end))
    {
    }

    constexpr qint64 start() const noexcept { return duration_cast<milliseconds>(s).count(); }
    constexpr qint64 end() const noexcept { return duration_cast<milliseconds>(e).count(); }
    constexpr qint64 duration() const noexcept { return duration_cast<milliseconds>(e - s).count(); }

    Q_INVOKABLE constexpr bool contains(qint64 time) const noexcept
    {
        return (start() <= time && time < end());
    }

    Q_INVOKABLE constexpr bool containedBy(qint64 startTime, qint64 endTime) const noexcept
    {
        return (startTime <= start() && endTime >= end());
    }

    constexpr Interval nextInterval(const microseconds& duration) const
    {
        return Interval(e, e + duration);
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
    friend QDebug inline operator<<(QDebug dbg, const Interval& interval);
    microseconds s = microseconds::zero();
    microseconds e = microseconds::zero();
};

QDebug inline operator<<(QDebug dbg, const Interval& interval)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "(" << interval.s << "/" << duration_cast<milliseconds>(interval.s) << ", " << interval.e << "/" << duration_cast<milliseconds>(interval.e) << ")";
    return dbg;
}
