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