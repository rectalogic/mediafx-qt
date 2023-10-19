#pragma once

#include <QAnimationDriver>

class AnimationDriver : public QAnimationDriver {
public:
    AnimationDriver(qint64 frameDuration, QObject* parent = nullptr)
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
        return m_elapsed;
    }

private:
    qint64 m_frameDuration;
    qint64 m_elapsed = 0;
};