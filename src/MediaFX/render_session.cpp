// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "render_session.h"
#include "animation.h"
#include "audio_renderer.h"
#include "formats.h"
#include "render_context.h"
#include "util.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMessageLogContext>
#include <QObject>
#include <QQmlEngine>
#include <QQmlInfo>
#include <QmlTypeAndRevisionsRegistration>

/*!
    \qmltype RenderSession
    //! \instantiates RenderSession
    \inqmlmodule MediaFX
    \brief The RenderSession singleton manages the current rendering session.

    Internally, RenderSession drives rendering the set of active MediaClips.
    It also exposes access to the current rendering time
    and allows the session to be shut down, see \l {RenderSession::endSession}.
*/

/*!
    \qmlproperty interval RenderSession::currentRenderTime

    The current frame interval being rendered, independent of any individual MediaClip.
*/

/*!
    \qmlmethod interval RenderSession::createInterval(int start, int end)

    Create a new interval using \a start and \a end times (milliseconds).

    \qml
    Item {
        property interval myInterval: RenderSession.createInterval(2000, 3000)
    }
    \endqml
*/

const QEvent::Type renderEventType = static_cast<const QEvent::Type>(QEvent::registerEventType());

RenderSession::RenderSession(QObject* parent)
    : QObject(parent)
{
}

RenderSession::~RenderSession()
{
    if (m_animationDriver)
        m_animationDriver->uninstall();
}

void RenderSession::initialize(const RenderContext& renderContext)
{
    if (m_renderContext) {
        qCritical() << "RenderSession is already initialized";
        emit qmlEngine(this)->exit(1);
        return;
    }
    m_renderContext = std::make_unique<RenderContext>(renderContext);
    m_currentRenderTime = Interval(0us, frameRateToFrameDuration<microseconds>(m_renderContext->frameRate()));
    m_rootAudioRenderer = std::make_unique<AudioRenderer>();

    m_animationDriver = std::make_unique<AnimationDriver>(frameRateToFrameDuration<microseconds>(m_renderContext->frameRate()));
    m_animationDriver->install();

    m_outputAudioFormat.setSampleFormat(AudioSampleFormat_Qt);
    m_outputAudioFormat.setChannelConfig(AudioChannelLayout_Qt);
    m_outputAudioFormat.setSampleRate(m_renderContext->sampleRate());
}

/*!
    \qmlmethod void RenderSession::pauseRendering

    Pause rendering to allow for asynchronous processing.
    \l resumeRendering must be called as many times as \c pauseRendering.
*/
void RenderSession::pauseRendering()
{
    m_pauseRendering++;
}

/*!
    \qmlmethod void RenderSession::resumeRendering

    Resume rendering after asynchronous processing.
    \c resumeRendering must be called as many times as \l pauseRendering.
*/
void RenderSession::resumeRendering()
{
    m_pauseRendering--;
    if (m_pauseRendering < 0) {
        qmlWarning(this) << "RenderSession resumeRendering called too many times";
        m_pauseRendering = 0;
    }
    if (m_pauseRendering == 0)
        postRenderEvent();
}

void RenderSession::render()
{
    if (isRenderingPaused())
        return;
    if (!m_isResumingRender)
        emit renderMediaClips();
    if (isRenderingPaused()) {
        m_isResumingRender = true;
        return;
    }
    m_isResumingRender = false;

    emit renderScene();

    m_frameCount++;
    m_currentRenderTime = m_currentRenderTime.nextInterval(duration_cast<microseconds>(m_frameCount * frameRateToFrameDuration(renderContext().frameRate())));
    emit currentRenderTimeChanged();

    if (isSessionEnded()) {
        emit sessionEnded();
        // Exit 0, the above slot should have exited with an error if necessary
        emit qmlEngine(this)->exit(0);
        return;
    }

    m_animationDriver->advance();

    postRenderEvent();
}

void RenderSession::beginSession()
{
    if (!m_renderContext) {
        qCritical() << "RenderSession is not initialized";
        emit qmlEngine(this)->exit(1);
        return;
    }
    postRenderEvent();
}

/*!
    \qmlmethod void RenderSession::endSession

    This method must be called to end the encoding session and exit.
    Typically this would be connected to the end of a MediaClip:

    \qml
    MediaClip {
        id: clip
        source: Qt.resolvedUrl("video.mp4")
        Component.onCompleted: {
            clip.clipEnded.connect(RenderSession.endSession);
        }
    }
    \endqml
*/
void RenderSession::endSession()
{
    m_sessionEnded = true;
}

void RenderSession::fatalError() const
{
    emit qmlEngine(this)->exit(1);
}

void RenderSession::postRenderEvent()
{
    if (m_isRenderEventPosted)
        return;
    m_isRenderEventPosted = true;
    QCoreApplication::postEvent(this, new QEvent(renderEventType)); // NOLINT(cppcoreguidelines-owning-memory)
}

bool RenderSession::event(QEvent* event)
{
    if (event->type() == renderEventType) {
        m_isRenderEventPosted = false;
        render();
        return true;
    }
    return QObject::event(event);
}

const QAudioBuffer& RenderSession::silentOutputAudioBuffer()
{
    if (!m_silentOutputAudioBuffer.isValid()) {
        m_silentOutputAudioBuffer = QAudioBuffer(m_outputAudioFormat.framesForDuration(frameRateToFrameDuration<microseconds>(renderContext().frameRate()).count()), m_outputAudioFormat);
    }
    return m_silentOutputAudioBuffer;
}
