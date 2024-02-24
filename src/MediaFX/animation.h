// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAnimationDriver>
#include <chrono>
using namespace std::chrono;

class AnimationDriver : public QAnimationDriver {
public:
    AnimationDriver(const microseconds& frameDuration, QObject* parent = nullptr)
        : QAnimationDriver(parent)
        , m_frameDuration(frameDuration)
    {
    }

    void advance() override
    {
        m_elapsed += m_frameDuration;
        advanceAnimation();
    }

    qint64 elapsed() const override
    {
        return duration_cast<milliseconds>(m_elapsed).count();
    }

private:
    microseconds m_frameDuration;
    microseconds m_elapsed = microseconds::zero();
};