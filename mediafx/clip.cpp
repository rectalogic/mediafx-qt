// Copyright (c) 2023 Andrew Wason. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clip.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMediaPlayer>
#include <QMessageLogContext>
#include <QUrl>

void Clip::setUrl(const QUrl& url)
{
    if (!m_url.isEmpty()) {
        qWarning() << "Clip url is a write-once property and cannot be changed";
        return;
    }
    m_url = url;
}

void Clip::setVideoSink(QVideoSink* videoSink)
{
    if (m_videoSink != videoSink) {
        m_videoSink = videoSink;
    }
}

void Clip::setClipEnd(qint64 clipEnd)
{
    if (m_clipEnd != clipEnd) {
        m_clipEnd = clipEnd;
        emit clipEndChanged();
        emit durationChanged();
    }
}

MediaClip::MediaClip()
    : Clip()
{
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &MediaClip::durationChanged);
    connect(&mediaPlayer, &QMediaPlayer::errorOccurred, this, &MediaClip::onErrorOccurred);
}

void MediaClip::onErrorOccurred(QMediaPlayer::Error error, const QString& errorString)
{
    qCritical() << "MediaClip error " << error << " " << errorString;
    QCoreApplication::exit(1);
}

void MediaClip::loadMedia(const QUrl& url)
{
    mediaPlayer.setSource(url);
}

void MediaClip::setClipBegin(qint64 clipBegin)
{
    if (m_clipBegin != clipBegin) {
        m_clipBegin = clipBegin;
        emit clipBeginChanged();
        emit durationChanged();
    }
}

qint64 MediaClip::duration() const
{
    if (clipEnd() != -1)
        return clipEnd() - clipBegin();
    else {
        return mediaPlayer.duration() - clipBegin();
    }
}

void ImageClip::loadMedia(const QUrl& url)
{
    if (!url.isLocalFile() || !image.load(url.toLocalFile())) {
        qCritical("ImageClip can only load local file urls");
        QCoreApplication::exit(1);
    }
}

qint64 ImageClip::duration() const
{
    auto ce = clipEnd();
    // XXX can we make this property required in the subclass?
    if (ce == -1)
        qCritical() << "ImageClip must have clipEnd set";
    return ce;
}