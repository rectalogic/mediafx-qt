// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "render_context.h"
#include <QObject>
#include <QQmlParserStatus>
#include <QSize>
#include <QString>
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
class OutputStream;
class QAudioBuffer;
struct AVFormatContext;
using namespace std::chrono;

class Encoder : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString outputFileName READ outputFileName WRITE setOutputFileName NOTIFY outputFileNameChanged FINAL REQUIRED)
    Q_PROPERTY(QSize frameSize READ frameSize WRITE setFrameSize NOTIFY frameSizeChanged FINAL REQUIRED)
    Q_PROPERTY(Rational frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged FINAL)
    Q_PROPERTY(int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged FINAL)
    QML_ELEMENT

public:
    using QObject::QObject;

    Encoder(QObject* parent = nullptr);
    Encoder(Encoder&&) = delete;
    Encoder& operator=(Encoder&&) = delete;
    ~Encoder() override;

    const QString& outputFileName() const { return m_outputFileName; }
    void setOutputFileName(const QString& outputFileName);

    const QSize& frameSize() const { return m_frameSize; }
    void setFrameSize(const QSize& frameSize);

    const Rational& frameRate() const { return m_frameRate; }
    void setFrameRate(const Rational& frameRate);

    int sampleRate() const { return m_sampleRate; }
    void setSampleRate(int sampleRate);

    void initialize();

signals:
    void outputFileNameChanged();
    void frameSizeChanged();
    void frameRateChanged();
    void sampleRateChanged();
    void encodingError();

public slots:
    bool encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData);
    bool finish();

protected:
    void classBegin() override {};
    void componentComplete() override;

private:
    Q_DISABLE_COPY(Encoder);

    bool m_isValid = false;
    QSize m_frameSize;
    int m_frameByteSize = 0;
    Rational m_frameRate = DefaultFrameRate;
    int m_sampleRate = DefaultSampleRate;
    QString m_outputFileName;
    AVFormatContext* m_formatContext = nullptr;
    std::unique_ptr<OutputStream> m_videoStream;
    std::unique_ptr<OutputStream> m_audioStream;
};