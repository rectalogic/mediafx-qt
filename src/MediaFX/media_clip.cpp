// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media_clip.h"
#include "audio_renderer.h"
#include "decoder.h"
#include "interval.h"
#include "render_context.h"
#include "render_session.h"
#include "util.h"
#include <QObject>
#include <QQmlEngine>
#include <QQmlInfo>
#include <QUrl>
#include <QVideoSink>
#include <QmlTypeAndRevisionsRegistration>
#include <chrono>
#include <compare>
#include <ratio>
using namespace std::chrono;
using namespace std::chrono_literals;

/*!
    \qmltype MediaClip
    //! \instantiates MediaClip
    \inqmlmodule MediaFX

    \brief Plays audio and video frames from a \l source.
*/
MediaClip::MediaClip(QObject* parent)
    : QObject(parent)
    , m_decoder(std::make_unique<Decoder>())
{
}

MediaClip::~MediaClip() = default;

/*!
    \qmlproperty url MediaClip::source

    The source media file url.
*/
void MediaClip::setSource(const QUrl& url)
{
    if (url.isEmpty()) {
        qmlWarning(this) << "MediaClip source is a write-once property and cannot be changed";
        return;
    }
    if (!url.isLocalFile()) {
        qmlWarning(this) << "MediaClip source requires a local file Url";
        return;
    }
    m_source = url;
    emit sourceChanged();
}

/*!
    \qmlproperty int MediaClip::startTime

    The start time offset (in milliseconds) of the clip.
    Defaults to 0.
*/
void MediaClip::setStartTime(qint64 ms)
{
    if (m_startTime != -1) {
        qmlWarning(this) << "MediaClip startTime is a write-once property and cannot be changed";
        return;
    }
    m_startTime = ms;

    std::chrono::duration<double> frameDuration(frameRateToFrameDuration(m_renderSession->frameRate()));
    m_startTimeAdjusted = duration_cast<microseconds>((milliseconds(ms) / frameDuration) * frameDuration);

    emit startTimeChanged();
    emit durationChanged();
}

/*!
    \qmlproperty int MediaClip::endTime

    The end time (in milliseconds) of the clip.
    Defaults to the clips duration.
*/
void MediaClip::setEndTime(qint64 ms)
{
    setEndTime(duration_cast<microseconds>(milliseconds(ms)));
}

void MediaClip::setEndTime(const microseconds& us)
{
    if (m_endTime != -1) {
        qmlWarning(this) << "MediaClip endTime is a write-once property and cannot be changed";
        return;
    }
    m_endTime = duration_cast<milliseconds>(us).count();

    std::chrono::duration<double> frameDuration(frameRateToFrameDuration(m_renderSession->frameRate()));
    m_endTimeAdjusted = duration_cast<microseconds>((us / frameDuration) * frameDuration);
    if (m_endTimeAdjusted < us)
        m_endTimeAdjusted += duration_cast<microseconds>(frameDuration);

    emit endTimeChanged();
    emit durationChanged();
}

/*!
    \qmlproperty int MediaClip::duration

    The duration (in milliseconds) of the clip.
    This is (\l endTime - \l startTime).
*/

/*!
    \qmlproperty int MediaClip::audioRenderer

    The \l AudioRenderer to render audio into.
*/
void MediaClip::setAudioRenderer(AudioRenderer* audioRenderer)
{
    if (audioRenderer != m_audioRenderer) {
        m_audioRenderer = audioRenderer;
        emit audioRendererChanged();
    }
}

void MediaClip::render()
{
    if (!isActive())
        return;

    if (!m_decoder->decode()) {
        m_renderSession->fatalError();
        return;
    }

    for (auto videoSink : m_videoSinks) {
        videoSink->setVideoFrame(m_decoder->outputVideoFrame());
    }
    if (m_audioRenderer && hasAudio())
        m_audioRenderer->addAudioBuffer(m_decoder->outputAudioBuffer());

    m_frameCount++;
    m_currentFrameTime = m_currentFrameTime.nextInterval(
        m_startTimeAdjusted + duration_cast<microseconds>(m_frameCount * frameRateToFrameDuration(m_renderSession->frameRate())));
    if (m_currentFrameTime.start() >= m_endTimeAdjusted) {
        emit clipEnded();
        m_decoder.reset();
        updateActive();
        return;
    }
    emit currentFrameTimeChanged();
}

/*!
    \qmlproperty bool MediaClip::active

    \c true if the clip is currently rendering video frames into a \l VideoRenderer.
*/
void MediaClip::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        emit activeChanged();
    }
}

void MediaClip::updateActive()
{
    // We are active if we are rendering video, or we have no video track but do have audio
    setActive(m_decoder && ((hasVideo() && !m_videoSinks.isEmpty()) || (!hasVideo() && hasAudio())));
}

void MediaClip::addVideoSink(QVideoSink* videoSink)
{
    if (videoSink && !m_videoSinks.contains(videoSink)) {
        m_videoSinks.append(videoSink);
        updateActive();
    }
}

void MediaClip::removeVideoSink(const QVideoSink* videoSink)
{
    if (videoSink && m_videoSinks.removeOne(videoSink)) {
        updateActive();
    }
}

void MediaClip::onDecoderErrorMessage(const QString& message)
{
    qmlWarning(this) << message << "(source" << source() << ")";
}

void MediaClip::loadMedia()
{
    if (!source().isValid()) {
        qmlWarning(this) << "MediaClip requires source Url";
        m_renderSession->fatalError();
        return;
    }
    connect(m_decoder.get(), &Decoder::errorMessage, this, &MediaClip::onDecoderErrorMessage);
    if (m_decoder->open(source().toLocalFile(), m_renderSession->frameRate(), m_renderSession->outputAudioFormat(), m_startTimeAdjusted) < 0) {
        m_renderSession->fatalError();
        return;
    }

    updateActive();
}

void MediaClip::classBegin()
{
    m_renderSession = RenderSession::findSession(this);
    if (m_renderSession) {
        connect(
            m_renderSession, &RenderSession::renderMediaClips,
            this, &MediaClip::render);
    } else {
        qmlWarning(this) << "MediaClip could not find renderSession in context";
        emit qmlEngine(this)->exit(1);
    }
}

void MediaClip::componentComplete()
{
    m_componentComplete = true;
    if (startTime() < 0)
        setStartTime(0);
    loadMedia();
    if (endTime() < 0) {
        setEndTime(m_decoder->duration());
    }
    m_currentFrameTime = Interval(m_startTimeAdjusted, m_startTimeAdjusted + frameRateToFrameDuration<microseconds>(m_renderSession->frameRate()));
    emit currentFrameTimeChanged();
}

/*!
    \qmlproperty interval MediaClip::currentFrameTime

    The time \l interval of the currently rendered video frame.
*/

/*!
    \qmlsignal MediaClip::clipEnded()

    This signal is emitted when the clip has finished playback.
*/