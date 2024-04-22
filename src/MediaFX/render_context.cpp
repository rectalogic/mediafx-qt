// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#include "render_context.h"

RenderContext::RenderContext(const QUrl& sourceUrl, const QString& outputFileName, const QSize& frameSize, const Rational& frameRate, int sampleRate, QObject* parent)
    : QObject(parent)
    , m_sourceUrl(sourceUrl)
    , m_outputFileName(outputFileName)
    , m_frameSize(frameSize)
    , m_frameRate(frameRate)
    , m_sampleRate(sampleRate)
{
}

void RenderContext::setSourceUrl(const QUrl& sourceUrl)
{
    m_sourceUrl = sourceUrl;
}

void RenderContext::setOutputFileName(const QString& outputFileName)
{
    m_outputFileName = outputFileName;
}

void RenderContext::setFrameSize(const QSize& frameSize)
{
    m_frameSize = frameSize;
}

void RenderContext::setFrameRate(const Rational& frameRate)
{
    m_frameRate = frameRate;
}

void RenderContext::setSampleRate(int sampleRate)
{
}