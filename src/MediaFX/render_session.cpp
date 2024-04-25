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
#include <QList>
#include <QObject>
#include <QPointer>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>
#include <QQmlInfo>
#include <QString>
#include <QVariant>
#include <QmlTypeAndRevisionsRegistration>
#include <QtLogging>
using namespace Qt::Literals::StringLiterals;

/*!
    \qmltype RenderSession
    //! \instantiates RenderSession
    \inqmlmodule MediaFX
    \brief The RenderSession manages the current rendering session.

    Internally, RenderSession drives rendering the set of active MediaClips.
    It also exposes access to the current rendering time
    and allows the session to be shut down, see \l {RenderSession::endSession}.
    The current session can be accessed using the \l {RenderSession::session} attached property.
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

const QString RenderSession::SessionContextProperty = u"renderSession"_s;
const QEvent::Type renderEventType = static_cast<const QEvent::Type>(QEvent::registerEventType());

RenderSession::RenderSession(QQuickItem* parent)
    : QQuickItem(parent)
    , m_rootAudioRenderer(std::make_unique<AudioRenderer>())
{
}

RenderSession::~RenderSession()
{
    if (m_animationDriver)
        m_animationDriver->uninstall();
}

RenderSession* RenderSession::findSession(QObject* object)
{
    QQmlContext* context = QQmlEngine::contextForObject(object);
    if (context)
        return context->contextProperty(SessionContextProperty).value<RenderSession*>();
    return nullptr;
}

/*!
    \qmlattachedproperty RenderSession RenderSession::session
    This property holds the current RenderSession.
    This allows the session to be accessed from any QML object.
*/
RenderSessionAttached* RenderSession::qmlAttachedProperties(QObject* object)
{
    RenderSession* session = findSession(object);
    if (session)
        return new RenderSessionAttached(session, object); // NOLINT(cppcoreguidelines-owning-memory)
    else
        return nullptr;
}

void RenderSession::componentComplete()
{
    QQuickItem::componentComplete();

    m_currentRenderTime = Interval(0us, frameRateToFrameDuration<microseconds>(frameRate()));
    m_animationDriver = std::make_unique<AnimationDriver>(frameRateToFrameDuration<microseconds>(frameRate()));
    m_animationDriver->install();

    m_outputAudioFormat.setSampleFormat(AudioSampleFormat_Qt);
    m_outputAudioFormat.setChannelConfig(AudioChannelLayout_Qt);
    m_outputAudioFormat.setSampleRate(sampleRate());

    QQmlComponent component(qmlEngine(this), m_sourceUrl);
    if (component.isError()) {
        for (auto& error : component.errors())
            qCritical() << error;
        fatalError();
        return;
    }
    QQmlContext* creationContext = component.creationContext();
    if (!creationContext)
        creationContext = qmlContext(this);
    QPointer<QQmlContext> context(new QQmlContext(creationContext, creationContext));
    // The RenderSession.session attached property uses this to find the session
    context->setContextProperty(SessionContextProperty, this);
    QObject* object = component.create(context);
    m_loadedItem = qobject_cast<QQuickItem*>(object);
    if (!m_loadedItem) {
        qmlWarning(this) << "Failed to load" << m_sourceUrl;
        fatalError();
        return;
    }
    m_loadedItem->setParent(this);
    m_loadedItem->setParentItem(this);
}

void RenderSession::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (m_loadedItem)
        m_loadedItem->setSize(newGeometry.size());
}

void RenderSession::setSourceUrl(const QUrl& url)
{
    if (m_sourceUrl != url) {
        if (!m_sourceUrl.isEmpty()) {
            qmlWarning(this) << "RenderSession sourceUrl is a write-once property";
            return;
        }
        m_sourceUrl = url;
        emit sourceUrlChanged();
    }
}

void RenderSession::setFrameRate(const Rational& frameRate)
{
    if (m_frameRate != frameRate) {
        if (m_frameRate != DefaultFrameRate) {
            qmlWarning(this) << "RenderSession frameRate is a write-once property";
            return;
        }
        m_frameRate = frameRate;
        emit frameRateChanged();
    }
}

void RenderSession::setSampleRate(int sampleRate)
{
    if (m_sampleRate != sampleRate) {
        if (m_sampleRate != DefaultSampleRate) {
            qmlWarning(this) << "RenderSession sampleRate is a write-once property";
            return;
        }
        m_sampleRate = sampleRate;
        emit sampleRateChanged();
    }
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
    m_currentRenderTime = m_currentRenderTime.nextInterval(duration_cast<microseconds>(m_frameCount * frameRateToFrameDuration(frameRate())));
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
    return QQuickItem::event(event);
}

const QAudioBuffer& RenderSession::silentOutputAudioBuffer()
{
    if (!m_silentOutputAudioBuffer.isValid()) {
        m_silentOutputAudioBuffer = QAudioBuffer(m_outputAudioFormat.framesForDuration(frameRateToFrameDuration<microseconds>(frameRate()).count()), m_outputAudioFormat);
    }
    return m_silentOutputAudioBuffer;
}
