// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDebug>
#include <QObject>
#include <QtAssert>
#include <QtQmlIntegration>
#include <QtTypes>
#include <chrono>
#include <compare>
using namespace std::chrono;

template <class T = microseconds>
class Interval {
public:
    constexpr Interval() noexcept = default;

    explicit constexpr Interval(const T& start, const T& end) noexcept
        : s(start)
        , e(end)
    {
        Q_ASSERT(s <= e);
    }
    template <class D>
    constexpr Interval(const Interval<D>& interval) noexcept
        : Interval(duration_cast<T>(interval.start()), duration_cast<T>(interval.end()))
    {
    }
    Interval(Interval&&) = default;
    Interval(const Interval&) = default;
    Interval& operator=(Interval&&) = default;
    Interval& operator=(const Interval&) = default;
    ~Interval() = default;

    constexpr const T& start() const noexcept { return s; }
    constexpr const T& end() const noexcept { return e; }
    constexpr T duration() const noexcept { return e - s; }

    constexpr bool contains(const T& time) const noexcept
    {
        return (start() <= time && time < end());
    }

    constexpr bool containedBy(const T& startTime, const T& endTime) const noexcept
    {
        return (startTime <= start() && endTime >= end());
    }

    constexpr Interval nextInterval(const T& endTime) const
    {
        return Interval(e, endTime);
    }

    friend constexpr bool operator==(Interval lhs, Interval rhs) noexcept
    {
        return lhs.start() == rhs.start() && lhs.end() == rhs.end();
    }
    friend constexpr bool operator!=(Interval lhs, Interval rhs) noexcept
    {
        return lhs.start() != rhs.start() || lhs.end() != rhs.end();
    }

    friend inline QDebug operator<<(QDebug dbg, const Interval& interval)
    {
        QDebugStateSaver saver(dbg);
        dbg.nospace() << "(" << interval.s << "/" << duration_cast<milliseconds>(interval.s) << ", " << interval.e << "/" << duration_cast<milliseconds>(interval.e) << ")";
        return dbg;
    }

private:
    T s { T::zero() };
    T e { T::zero() };
};

class IntervalGadget {
    Q_GADGET
    QML_VALUE_TYPE(interval)
    Q_PROPERTY(int start READ start FINAL)
    Q_PROPERTY(int end READ end FINAL)
    Q_PROPERTY(int duration READ duration FINAL)

public:
    constexpr IntervalGadget() noexcept = default;
    explicit constexpr IntervalGadget(qint64 start, qint64 end) noexcept
        : m_interval(milliseconds(start), milliseconds(end))
    {
    }
    explicit constexpr IntervalGadget(const Interval<microseconds>& interval) noexcept
        : m_interval(interval)
    {
    }
    IntervalGadget(IntervalGadget&&) = default;
    IntervalGadget(const IntervalGadget&) = default;
    IntervalGadget& operator=(IntervalGadget&&) = default;
    IntervalGadget& operator=(const IntervalGadget&) = default;
    ~IntervalGadget() = default;

    constexpr qint64 start() const noexcept { return duration_cast<milliseconds>(m_interval.start()).count(); }
    constexpr qint64 end() const noexcept { return duration_cast<milliseconds>(m_interval.end()).count(); }
    constexpr qint64 duration() const noexcept { return duration_cast<milliseconds>(m_interval.duration()).count(); }

    Q_INVOKABLE constexpr bool contains(qint64 time) const noexcept
    {
        return m_interval.contains(milliseconds(time));
    }

    Q_INVOKABLE constexpr bool containedBy(qint64 startTime, qint64 endTime) const noexcept
    {
        return m_interval.containedBy(milliseconds(startTime), milliseconds(endTime));
    }

    friend constexpr bool operator==(IntervalGadget lhs, IntervalGadget rhs) noexcept
    {
        return lhs.m_interval == rhs.m_interval;
    }
    friend constexpr bool operator!=(IntervalGadget lhs, IntervalGadget rhs) noexcept
    {
        return lhs.m_interval != rhs.m_interval;
    }

    friend inline QDebug operator<<(QDebug dbg, const IntervalGadget& interval)
    {
        dbg << interval.m_interval;
        return dbg;
    }

private:
    Interval<microseconds> m_interval;
};