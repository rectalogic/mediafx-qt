// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "encoder.h"
#include <QObject>
#include <QStringLiteral>
#include <QtTest>
#include <chrono>
using namespace std::chrono;

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
        QCOMPARE(Encoder::FrameRate(30, 1).toFrameDuration(), 33333us);
    }
};

QTEST_APPLESS_MAIN(tst_Encoder);
#include "tst_encoder.moc"