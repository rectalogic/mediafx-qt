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

#include "encoder.h"
#include <QObject>
#include <QStringLiteral>
#include <QtTest>

class tst_Encoder : public QObject {
    Q_OBJECT

private slots:

    void frameSize()
    {
        QCOMPARE(Encoder::FrameSize::parse("640x360"), Encoder::FrameSize(640, 360));
        QCOMPARE(Encoder::FrameSize(640, 360).toString(), QStringLiteral("640x360"));
        QVERIFY(Encoder::FrameSize().isEmpty());
    }

    void frameRate()
    {
        QCOMPARE(Encoder::FrameRate::parse("30"), Encoder::FrameRate(30, 1));
        QCOMPARE(Encoder::FrameRate::parse("30000/1001"), Encoder::FrameRate(30000, 1001));
        QCOMPARE(Encoder::FrameRate(30000, 1001).toString(), QStringLiteral("30000/1001"));
        QCOMPARE(Encoder::FrameRate(30, 1).toString(), QStringLiteral("30"));
        QVERIFY(Encoder::FrameRate().isEmpty());
        QCOMPARE(Encoder::FrameRate(30, 1).toFrameDurationMicros(), 33333);
    }
};

QTEST_APPLESS_MAIN(tst_Encoder);
#include "tst_encoder.moc"