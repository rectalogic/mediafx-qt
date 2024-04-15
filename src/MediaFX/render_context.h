// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QtQmlIntegration>
extern "C" {
#include <libavutil/rational.h>
}

class RenderContext {
    Q_GADGET
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl CONSTANT)
    Q_PROPERTY(QString outputFileName READ outputFileName CONSTANT)
    Q_PROPERTY(QSize frameSize READ frameSize CONSTANT)
    QML_UNCREATABLE("")
    QML_VALUE_TYPE(renderContext)
public:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    explicit RenderContext(const QUrl& sourceUrl = QUrl(), const QString& outputFileName = "", const QSize& frameSize = QSize(640, 360), const AVRational& frameRate = { 30, 1 }, int sampleRate = 44100);

    RenderContext(RenderContext&&) = default;
    RenderContext(const RenderContext&) = default;
    RenderContext& operator=(RenderContext&&) = default;
    RenderContext& operator=(const RenderContext&) = default;
    ~RenderContext() = default;

    constexpr const QUrl& sourceUrl() const { return m_sourceUrl; }
    constexpr const QString& outputFileName() const { return m_outputFileName; }
    constexpr const QSize& frameSize() const noexcept { return m_frameSize; }
    constexpr const AVRational& frameRate() const noexcept { return m_frameRate; }
    constexpr int sampleRate() const noexcept { return m_sampleRate; }

private:
    QSize m_frameSize;
    AVRational m_frameRate;
    int m_sampleRate;
    QUrl m_sourceUrl;
    QString m_outputFileName;
};
