// Copyright (C) 2023 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media_manager.h"
#include "audio_renderer.h"
#include "formats.h"
#include "media_clip.h"
#include "video_track.h"
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QObject>
#include <QQmlEngine>
#include <QQuickView>
#include <chrono>
using namespace std::chrono;

/*!
    \qmltype MediaManager
    //! \instantiates MediaManager
    \inqmlmodule MediaFX
    \brief The MediaManager singleton provides access to the current rendering time interval.

    Internally, MediaManager manages the set of active MediaClips.
    It also exposes access to the \l Window hosting the QML, and the current rendering time.
*/

/*!
    \qmlproperty interval MediaManager::currentRenderTime

    The current frame interval being rendered, independent of any individual MediaClip.
*/

/*!
    \qmlproperty Window MediaManager::window

    Provides access to the \l Window hosting the QML.

    For example, to set the \l [QML]{Window::color} to a palette entry:
    \qml
    Item {
        Component.onCompleted: MediaManager.window.color = MediaManager.window.palette.window
    }
    \endqml
*/

/*!
    \qmlmethod interval MediaManager::createInterval(int start, int end)

    Create a new interval using \a start and \a end times (milliseconds).

    \qml
    Item {
        property interval myInterval: MediaManager.createInterval(2000, 3000)
    }
    \endqml
*/

MediaManager::MediaManager(const microseconds& outputVideoFrameDuration, int outputAudioSampleRate, QQuickView* quickView, QObject* parent)
    : QObject(parent)
    , m_outputVideoFrameDuration(outputVideoFrameDuration)
    , m_currentRenderTime(Interval(0us, outputVideoFrameDuration))
    , m_quickView(quickView)
{
    m_outputAudioFormat.setSampleFormat(AudioSampleFormat_Qt);
    m_outputAudioFormat.setChannelConfig(AudioChannelLayout_Qt);
    m_outputAudioFormat.setSampleRate(outputAudioSampleRate);
}

MediaManager::~MediaManager() = default;

void MediaManager::initialize()
{
    MediaManagerForeign::setSingletonInstance(this);
    // AudioRenderer depends on the singleton, so create it after initializing singleton
    m_rootAudioRenderer = std::make_unique<AudioRenderer>(true);
}

MediaManager* MediaManager::singletonInstance()
{
    return MediaManagerForeign::singletonInstance();
}

QAudioBuffer MediaManager::createOutputAudioBuffer()
{
    return QAudioBuffer(m_outputAudioFormat.framesForDuration(outputVideoFrameDuration().count()), m_outputAudioFormat);
}

void MediaManager::registerClip(MediaClip* clip)
{
    if (clip && !activeClips.contains(clip)) {
        activeClips.append(clip);
    }
}

void MediaManager::unregisterClip(MediaClip* clip)
{
    activeClips.removeOne(clip);
}

void MediaManager::updateVideoSinks(MediaClip* oldClip, MediaClip* newClip, QVideoSink* videoSink)
{
    if (oldClip) {
        VideoTrack* videoTrack = oldClip->videoTrack();
        if (videoTrack) {
            videoTrack->removeVideoSink(videoSink);
        }
    }
    if (newClip) {
        VideoTrack* videoTrack = newClip->videoTrack();
        if (videoTrack) {
            videoTrack->addVideoSink(videoSink);
        }
    }
}

void MediaManager::nextRenderTime()
{
    m_currentRenderTime = m_currentRenderTime.nextInterval(m_outputVideoFrameDuration);
    emit currentRenderTimeChanged();
}

void MediaManager::render()
{
    for (auto clip : activeClips) {
        clip->render();
    }
}

/*!
    \qmlmethod void MediaManager::finishEncoding

    This method must be called to end the encoding session and exit.
    Typically this would be connected to the end of a MediaClip:

    \qml
    MediaClip {
        id: clip
        source: Qt.resolvedUrl("video.mp4")
        Component.onCompleted: {
            clip.clipEnded.connect(MediaManager.finishEncoding);
        }
    }
    \endqml
*/
void MediaManager::finishEncoding()
{
    finishedEncoding = true;
}

void MediaManager::fatalError() const
{
    emit window()->engine()->exit(1);
}
