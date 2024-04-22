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

struct Rational : public AVRational {
    Q_GADGET
public:
    friend constexpr bool operator==(const Rational& lhs, const Rational& rhs) noexcept
    {
        return lhs.num == rhs.num && lhs.den == rhs.den;
    }
    friend constexpr bool operator!=(const Rational& lhs, const Rational& rhs) noexcept
    {
        return lhs.num != rhs.num || lhs.den != rhs.den;
    }
};

inline constexpr int DefaultSampleRate = 44100;
inline constexpr Rational DefaultFrameRate = { 30, 1 };

class RenderContext : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl CONSTANT)
    Q_PROPERTY(QString outputFileName READ outputFileName CONSTANT)
    Q_PROPERTY(int sampleRate READ sampleRate CONSTANT)
    Q_PROPERTY(QSize frameSize READ frameSize CONSTANT)
    Q_PROPERTY(Rational frameRate READ frameRate CONSTANT)
    QML_ELEMENT
    QML_SINGLETON
public:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    explicit RenderContext(const QUrl& sourceUrl = QUrl(), const QString& outputFileName = "", const QSize& frameSize = QSize(640, 360), const Rational& frameRate = DefaultFrameRate, int sampleRate = DefaultSampleRate, QObject* parent = nullptr);
    RenderContext(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    ~RenderContext() override = default;

    constexpr const QUrl& sourceUrl() const { return m_sourceUrl; }
    void setSourceUrl(const QUrl& sourceUrl);
    constexpr const QString& outputFileName() const { return m_outputFileName; }
    void setOutputFileName(const QString& outputFileName);
    constexpr const QSize& frameSize() const noexcept { return m_frameSize; }
    void setFrameSize(const QSize& frameSize);
    constexpr const Rational& frameRate() const noexcept { return m_frameRate; }
    void setFrameRate(const Rational& frameRate);
    constexpr int sampleRate() const noexcept { return m_sampleRate; }
    void setSampleRate(int sampleRate);

private:
    Q_DISABLE_COPY(RenderContext);
    QSize m_frameSize;
    Rational m_frameRate;
    int m_sampleRate;
    QUrl m_sourceUrl;
    QString m_outputFileName;
};
