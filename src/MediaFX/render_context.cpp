// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#include "render_context.h"
extern "C" {
#include <libavutil/rational.h>
}

RenderContext::RenderContext(const QUrl& sourceUrl, const QString& outputFileName, const QSize& frameSize, const AVRational& frameRate, int sampleRate)
    : m_sourceUrl(sourceUrl)
    , m_outputFileName(outputFileName)
    , m_frameSize(frameSize)
    , m_frameRate(frameRate)
    , m_sampleRate(sampleRate)
{
}