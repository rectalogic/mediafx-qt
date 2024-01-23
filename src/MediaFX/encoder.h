// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QByteArray>
#include <QObject>
#include <QSize>
#include <QString>
#include <QStringLiteral>
#include <chrono>
#include <stdio.h>
class QAudioBuffer;
using namespace std::chrono;

class Encoder : public QObject {
    Q_OBJECT

public:
    class FrameSize {
    public:
        constexpr FrameSize() noexcept = default;
        explicit constexpr FrameSize(int width, int height)
            : m_width(width)
            , m_height(height) {};
        constexpr int width() const noexcept { return m_width; };
        constexpr int height() const noexcept { return m_height; };
        constexpr bool isEmpty() const noexcept { return m_width == 0 || m_height == 0; };
        constexpr QSize toSize() const noexcept { return QSize(m_width, m_height); };
        static FrameSize parse(const QString& fs)
        {
            int w = 0, h = 0;
            if (sscanf(fs.toUtf8().constData(), "%dx%d", &w, &h) == 2 && w > 0 && h > 0) {
                return FrameSize(w, h);
            }
            return FrameSize();
        };
        QString toString() const
        {
            return QStringLiteral("%1x%2").arg(QString::number(m_width), QString::number(m_height));
        };
        friend constexpr bool operator==(FrameSize lhs, FrameSize rhs) noexcept
        {
            return lhs.width() == rhs.width() && lhs.height() == rhs.height();
        };
        friend constexpr bool operator!=(FrameSize lhs, FrameSize rhs) noexcept
        {
            return lhs.width() != rhs.width() || lhs.height() != rhs.height();
        };

    private:
        int m_width = 0;
        int m_height = 0;
    };

    class FrameRate {
    public:
        constexpr FrameRate() noexcept = default;
        explicit constexpr FrameRate(int num, int den)
            : m_num(num)
            , m_den(den) {};
        constexpr int num() const noexcept { return m_num; };
        constexpr int den() const noexcept { return m_den; };
        constexpr bool isEmpty() const noexcept { return m_num == 0 || m_den == 0; };
        constexpr double toDouble() const noexcept { return m_num / (double)m_den; };
        constexpr microseconds toFrameDuration() const noexcept { return round<microseconds>(duration<double> { 1 / toDouble() }); }
        static FrameRate parse(const QString& fr)
        {
            float num = 0, den = 0;
            num = fr.toDouble();
            if (num > 0) {
                return FrameRate(num, 1);
            } else {
                if (sscanf(fr.toUtf8().constData(), "%f/%f", &num, &den) == 2 && num > 0 && den > 0) {
                    return FrameRate(num, den);
                }
            }
            return FrameRate();
        }
        QString toString() const
        {
            if (m_den == 1) {
                return QString::number(m_num);
            }
            return QStringLiteral("%1/%2").arg(QString::number(m_num), QString::number(m_den));
        };
        friend constexpr bool operator==(FrameRate lhs, FrameRate rhs) noexcept
        {
            return lhs.num() == rhs.num() && lhs.den() == rhs.den();
        };
        friend constexpr bool operator!=(FrameRate lhs, FrameRate rhs) noexcept
        {
            return lhs.num() != rhs.num() || lhs.den() != rhs.den();
        };

    private:
        int m_num = 0;
        int m_den = 0;
    };

    Encoder(const FrameSize& outputFrameSize, const FrameRate& outputFrameRate, int outputSampleRate, QObject* parent = nullptr)
        : QObject(parent)
        , m_outputFrameSize(outputFrameSize)
        , m_outputFrameRate(outputFrameRate)
        , m_outputSampleRate(outputSampleRate) {};
    ~Encoder();
    bool initialize(const QString& output, const QString& command);
    constexpr FrameSize outputFrameSize() const noexcept { return m_outputFrameSize; };
    constexpr FrameRate outputFrameRate() const noexcept { return m_outputFrameRate; };
    constexpr int outputSampleRate() const noexcept { return m_outputSampleRate; };

    bool encode(const QAudioBuffer& audioBuffer, const QByteArray& videoData);

private:
    FrameSize m_outputFrameSize;
    FrameRate m_outputFrameRate;
    int m_outputSampleRate;
    int m_pid = -1;
    int m_audiofd = -1;
    int m_videofd = -1;
};