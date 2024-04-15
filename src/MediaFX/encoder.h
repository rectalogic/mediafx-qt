// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QString>
#include <QtCore>
#include <QtQmlIntegration>
#include <chrono>
#include <memory>
Q_MOC_INCLUDE("output_stream.h")
class RenderContext;
class OutputStream;
class QAudioBuffer;
struct AVFormatContext;
using namespace std::chrono;

class Encoder : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString outputFileName READ outputFileName WRITE setOutputFileName NOTIFY outputFileNameChanged FINAL)
    QML_ELEMENT

public:
    using QObject::QObject;

    Encoder(QObject* parent = nullptr);
    Encoder(Encoder&&) = delete;
    Encoder& operator=(Encoder&&) = delete;
    ~Encoder() override;

    void initialize(const RenderContext& renderContext);
    const QString& outputFileName() const { return m_outputFileName; }
    void setOutputFileName(const QString& outputFileName);

signals:
    void outputFileNameChanged();
    void encodingError();

public slots:
    bool encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData);
    bool finish();

protected:
    void classBegin() override { }
    void componentComplete() override;

private:
    Q_DISABLE_COPY(Encoder);

    bool m_isValid = false;
    QString m_outputFileName;
    AVFormatContext* m_formatContext = nullptr;
    std::unique_ptr<OutputStream> m_videoStream;
    std::unique_ptr<OutputStream> m_audioStream;
};