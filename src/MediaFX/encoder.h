// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QtCore>
#include <chrono>
#include <memory>
Q_MOC_INCLUDE("output_stream.h")
class OutputFormat;
class OutputStream;
class QAudioBuffer;
struct AVFormatContext;
using namespace std::chrono;

class Encoder : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    Encoder(const QString& outputFile, const OutputFormat& outputFormat);
    Encoder(Encoder&&) = delete;
    Encoder& operator=(Encoder&&) = delete;
    ~Encoder() override;
    bool isValid() const { return m_isValid; }

signals:
    void encodingError();

public slots:
    bool encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData);
    bool finish();

private:
    Q_DISABLE_COPY(Encoder);

    bool m_isValid = false;
    QString m_outputFile;
    AVFormatContext* m_formatContext = nullptr;
    std::unique_ptr<OutputStream> m_videoStream;
    std::unique_ptr<OutputStream> m_audioStream;
};