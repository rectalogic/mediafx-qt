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

using namespace std::chrono_literals;

class tst_Interval : public QObject {
    Q_OBJECT

private slots:

    void contains()
    {
        QVERIFY(Interval(100us, 200us).contains(150us));
        QVERIFY(!Interval(100us, 200us).contains(300us));
        QVERIFY(Interval(100us, 200us).contains(100us));
        QVERIFY(!Interval(100us, 200us).contains(200us));
    }

    void translated()
    {
        QCOMPARE(Interval(33333us, 66666us), Interval(0us, 33333us).translated(33333us));
    }

    void qdebug()
    {
        QString result;
        QDebug { &result } << Interval(100us, 200us);
        QCOMPARE(result, QStringLiteral("(100us, 200us) "));
    }
};

QTEST_APPLESS_MAIN(tst_Interval);
#include "tst_interval.moc"