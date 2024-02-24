// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "decoder.h"
#include "formats.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileDevice>
#include <QIODeviceBase>
#include <QObject>
#include <QString>
#include <QTestData>
#include <QVideoFrame>
#include <QtLogging>
#include <QtTest>
#include <chrono>
#include <memory>
extern "C" {
#include <libavutil/rational.h>
}
using namespace std::chrono_literals;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

class RawWriter {
public:
    RawWriter() = default;

    bool open(Decoder* decoder, const QString& videoPath, const QString& audioPath)
    {
        QFile* videoFile = openRawStream(videoPath, decoder);
        QFile* audioFile = openRawStream(audioPath, decoder);
        if (!(videoFile && audioFile))
            return false;
        m_rawVideoData = std::make_unique<QDataStream>(videoFile);
        m_rawAudioData = std::make_unique<QDataStream>(audioFile);
        m_isOpen = true;
        return true;
    }

    void write(QVideoFrame& videoFrame, const QAudioBuffer& audioBuffer)
    {
        if (!m_isOpen)
            return;
        videoFrame.map(QVideoFrame::ReadOnly);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        m_rawVideoData->writeRawData(reinterpret_cast<const char*>(videoFrame.bits(0)), videoFrame.mappedBytes(0));
        videoFrame.unmap();
        if (audioBuffer.isValid())
            m_rawAudioData->writeRawData(audioBuffer.constData<char>(), audioBuffer.byteCount()); // NOLINT(*-narrowing-conversions)
    }

private:
    QFile* openRawStream(const QString& path, QObject* parent)
    {
        auto file = std::make_unique<QFile>(path);
        if (!file->open(QIODeviceBase::WriteOnly, QFileDevice::ReadOwner | QFileDevice::WriteOwner))
            return nullptr;
        auto f = file.release();
        f->setParent(parent);
        return f;
    }

    bool m_isOpen = false;
    std::unique_ptr<QDataStream> m_rawVideoData;
    std::unique_ptr<QDataStream> m_rawAudioData;
};

class tst_Decoder : public QObject {
    Q_OBJECT

public slots:
    void onDecoderError(const QString& message)
    {
        qCritical() << message;
    }

private slots:
    void decode_data()
    {
        QTest::addColumn<QString>("inputPath");
        QTest::addColumn<AVRational>("frameRate");
        QTest::addColumn<int>("videoFrameCount");
        QTest::addColumn<int>("audioFrameCount");

        QTest::newRow("15fps red-320x180-15fps-8s-kal1624000.nut") << QFINDTESTDATA("fixtures/assets/red-320x180-15fps-8s-kal1624000.nut") << AVRational { 15, 1 } << 121 << 120;
        QTest::newRow("30fps red-320x180-15fps-8s-kal1624000.nut") << QFINDTESTDATA("fixtures/assets/red-320x180-15fps-8s-kal1624000.nut") << AVRational { 30, 1 } << 242 << 241;
        QTest::newRow("30fps red-160x120.png") << QFINDTESTDATA("fixtures/assets/red-160x120.png") << AVRational { 30, 1 } << 2 << 0;
    }

    void decode()
    {
        QFETCH(QString, inputPath);
        QFETCH(AVRational, frameRate);
        QFETCH(int, audioFrameCount);
        QFETCH(int, videoFrameCount);

        QAudioFormat audioFormat;
        audioFormat.setSampleFormat(AudioSampleFormat_Qt);
        audioFormat.setChannelConfig(AudioChannelLayout_Qt);
        audioFormat.setSampleRate(44100);

        Decoder decoder;
        connect(&decoder, &Decoder::errorMessage, this, &tst_Decoder::onDecoderError);
        QVERIFY(decoder.open(inputPath, frameRate, audioFormat, 0s) >= 0);

        RawWriter writer;
#if 0
        // Dump raw decoded data for analysis
        av_log_set_level(AV_LOG_DEBUG);
        QVERIFY(writer.open(&decoder, inputPath + ".video.raw", inputPath + ".audio.raw"));
#endif

        int audioFrames = 0;
        int videoFrames = 0;
        while (!decoder.isAudioEOF() || !decoder.isVideoEOF()) {
            QVERIFY(decoder.decode());
            QVideoFrame videoFrame(decoder.outputVideoFrame());
            QAudioBuffer audioBuffer(decoder.outputAudioBuffer());
            writer.write(videoFrame, audioBuffer);
            videoFrames++;
            if (audioBuffer.isValid())
                audioFrames++;
        }
        QCOMPARE(audioFrames, audioFrameCount);
        QCOMPARE(videoFrames, videoFrameCount);
    }
};

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

QTEST_APPLESS_MAIN(tst_Decoder);
#include "tst_decoder.moc"