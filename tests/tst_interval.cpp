// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "interval.h"
#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QtTest>
#include <chrono>
using namespace std::chrono;
using namespace std::chrono_literals;

class tst_Interval : public QObject {
    Q_OBJECT

private slots:

    void contains()
    {
        QVERIFY(Interval<microseconds>(100ms, 200ms).contains(150ms));
        QVERIFY(!Interval<microseconds>(100ms, 200ms).contains(300ms));
        QVERIFY(Interval<microseconds>(100ms, 200ms).contains(100ms));
        QVERIFY(!Interval<microseconds>(100ms, 200ms).contains(200ms));
    }

    void conversion()
    {
        Interval<microseconds> i1(100ms, 200ms);
        Interval<nanoseconds> i2(i1);
        QCOMPARE(i2.start(), i1.start());
        QCOMPARE(i2.end(), i1.end());
        Interval<milliseconds> i3(i1);
        QCOMPARE(i3.start(), i1.start());
        QCOMPARE(i3.end(), i1.end());
    }

    void nextInterval()
    {
        QCOMPARE(Interval<microseconds>(0ms, 3333ms).nextInterval(6666000us), Interval<microseconds>(3333ms, 6666ms));
    }

    void qdebug()
    {
        QString result;
        QDebug dbg { &result };
        dbg << Interval<microseconds>(100ms, 200ms); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        QCOMPARE(result, QStringLiteral("(100000us/100ms, 200000us/200ms) "));
    }
};

QTEST_APPLESS_MAIN(tst_Interval);
#include "tst_interval.moc"