// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "encoder.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QByteArray>
#include <QChar>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QSysInfo>
#include <QtCore>
#include <QtTest>
#include <chrono>
#include <math.h>
#include <stdint.h>
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

    void encode()
    {
        QString uname(QSysInfo::kernelType());
        uname.replace(0, 1, uname[0].toUpper());
        QDir outputDir(QFINDTESTDATA("../"));
        QString buildDir = "build/" + uname + "/output";
        outputDir.mkpath(buildDir);
        outputDir.setPath(outputDir.absoluteFilePath(buildDir));

        QFile encodedFile(outputDir.filePath("encoder.nut"));

        int sampleRate = 44100;
        auto frameRate = Encoder::FrameRate(5, 1);
        auto frameSize = Encoder::FrameSize(160, 120);

        Encoder encoder(encodedFile.fileName(), frameSize, frameRate, sampleRate);
        QVERIFY(encoder.initialize());

        QAudioFormat audioFormat;
        audioFormat.setSampleFormat(QAudioFormat::Float);
        audioFormat.setChannelConfig(QAudioFormat::ChannelConfigStereo);
        audioFormat.setSampleRate(sampleRate);
        QAudioBuffer audioBuffer(audioFormat.framesForDuration(frameRate.toFrameDuration().count()), audioFormat);

        QByteArray videoData(frameSize.width() * frameSize.height() * 4, Qt::Uninitialized);

        int frames = 2.0 * frameRate.toDouble(); // 2 seconds
        double audioTime = 0;
        double audioIncr = 2 * M_PI * 110.0 / sampleRate;
        double audioIncr2 = 2 * M_PI * 110.0 / sampleRate / sampleRate;

        for (int i = 0; i < frames; i++) {
            // Audio sine wave
            float* q = audioBuffer.data<float>();
            for (int j = 0; j < audioBuffer.frameCount(); j++) {
                float v = sin(audioTime);
                for (int c = 0; c < audioFormat.channelCount(); c++)
                    *q++ = v;
                audioTime += audioIncr;
                audioIncr += audioIncr2;
            }
            // Video RGBA colors
            int step = frameRate.toDouble() * i;
            for (int y = 0; y < frameSize.height(); y++) {
                for (int x = 0; x < frameSize.width(); x++) {
                    uint8_t* pixel = reinterpret_cast<uint8_t*>(&(videoData.data()[(y * frameSize.width() + x) * 4]));
                    pixel[0] = x + y + step * 3;
                    pixel[1] = 128 + y + step * 2;
                    pixel[2] = 64 + x + step * 5;
                    pixel[3] = 0xff; // alpha
                }
            }

            QVERIFY(encoder.encode(audioBuffer, videoData));
        }
        QVERIFY(encoder.finish());

        QVERIFY(encodedFile.open(QIODevice::ReadOnly));
        QByteArray encodedData(encodedFile.readAll());
        encodedFile.close();

        QString fixturePath = QFINDTESTDATA("fixtures/encoder.nut");
        QFile fixtureFile(fixturePath);
        QVERIFY(fixtureFile.open(QIODevice::ReadOnly));
        QByteArray fixtureData(fixtureFile.readAll());
        fixtureFile.close();

        QCOMPARE(fixtureData, encodedData);
    }
};

QTEST_APPLESS_MAIN(tst_Encoder);
#include "tst_encoder.moc"