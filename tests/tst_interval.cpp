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
        QVERIFY(Interval(100, 200).contains(150));
        QVERIFY(!Interval(100, 200).contains(300));
        QVERIFY(Interval(100, 200).contains(100));
        QVERIFY(!Interval(100, 200).contains(200));
    }

    void nextInterval()
    {
        QCOMPARE(Interval(3333, 6666), Interval(0, 3333).nextInterval(3333000us));
    }

    void qdebug()
    {
        QString result;
        QDebug { &result } << Interval(100, 200);
        QCOMPARE(result, QStringLiteral("(100000us/100ms, 200000us/200ms) "));
    }
};

QTEST_APPLESS_MAIN(tst_Interval);
#include "tst_interval.moc"