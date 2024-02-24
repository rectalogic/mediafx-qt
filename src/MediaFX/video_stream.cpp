// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video_stream.h"
#include "formats.h"
#include "util.h"
#include <QObject>
#include <QSize>
#include <QString>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QtCore>
#include <array>
#include <cstring>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/version.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <libavutil/version.h>
}
using namespace Qt::Literals::StringLiterals;

// NOLINTBEGIN(bugprone-assignment-in-if-condition)

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void VideoStream::createBuffers(const AVFilter** bufferSrc, const AVFilter** bufferSink)
{
    *bufferSrc = avfilter_get_by_name("buffer");
    *bufferSink = avfilter_get_by_name("buffersink");
}

QString VideoStream::createBufferSrcArgs(const AVRational& timeBase)
{
    auto videoCodecContext = codecContext();
    QString args = u"video_size=%1x%2:pix_fmt=%3:time_base=%4/%5:pixel_aspect=%6/%7"_s.arg(
        QString::number(videoCodecContext->width), QString::number(videoCodecContext->height),
        QString::number(videoCodecContext->pix_fmt),
        QString::number(timeBase.num), QString::number(timeBase.den),
        QString::number(videoCodecContext->sample_aspect_ratio.num), QString::number(videoCodecContext->sample_aspect_ratio.den));
#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(9, 16, 100)
    args.append(u":colorspace=%1:range=%2"_s.arg(
        QString::number(videoCodecContext->colorspace),
        QString::number(videoCodecContext->color_range)));
#endif
    if (videoCodecContext->framerate.num != 0) {
        args.append(u":frame_rate=%1/%2"_s.arg(
            QString::number(videoCodecContext->framerate.num), QString::number(videoCodecContext->framerate.den)));
    }
    return args;
}

int VideoStream::configureBufferSink()
{
    int ret = 0;
    std::array<enum AVPixelFormat, 2> pixelFormats { VideoPixelFormat_FFMPEG, AV_PIX_FMT_NONE };
    if ((ret = av_opt_set_int_list(bufferSinkContext(), "pix_fmts", pixelFormats.data(), AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output video pixel format: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(9, 16, 100)
    std::array<enum AVColorSpace, 2> colorSpaces { VideoColorSpace_FFMPEG, AVCOL_SPC_UNSPECIFIED };
    std::array<enum AVColorRange, 2> colorRanges { VideoColorRange_FFMPEG, AVCOL_RANGE_UNSPECIFIED };
    if ((ret = av_opt_set_int_list(bufferSinkContext(), "color_spaces", colorSpaces.data(), AVCOL_SPC_UNSPECIFIED, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output video colorspace: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
    if ((ret = av_opt_set_int_list(bufferSinkContext(), "color_ranges", colorRanges.data(), AVCOL_RANGE_UNSPECIFIED, AV_OPT_SEARCH_CHILDREN)) < 0) {
        emit errorMessage(u"Failed to set output video color range: %1"_s.arg(av_err2qstring(ret)));
        return ret;
    }
#endif
    return ret;
}

QString VideoStream::configureFilters()
{
    return u"fps=%1/%2:start_time=0,scale=w=0:h=-1"_s.arg(
        QString::number(m_outputFrameRate.num), QString::number(m_outputFrameRate.den));
}

void VideoStream::processFrame(AVFrame* frame)
{
    Stream::processFrame(frame);
    if (!frame) {
        return;
    }
    // Swap frames because VideoOutput just compares internal frame pointers and ignores if the same
    std::swap(m_videoFrames.first, m_videoFrames.second);
    QVideoFrame& videoFrame = m_videoFrames.first;
    QVideoFrameFormat format = videoFrame.surfaceFormat();
    if (format.frameHeight() != frame->height || format.frameWidth() != frame->width) {
        QVideoFrameFormat newFormat(QSize(frame->width, frame->height), VideoPixelFormat_Qt);
        // XXX newFormat.setColorTransfer() from frame->color_trc?
        newFormat.setColorSpace(VideoColorSpace_Qt);
        newFormat.setColorRange(VideoColorRange_Qt);
        videoFrame = QVideoFrame(newFormat);
    }

    videoFrame.map(QVideoFrame::WriteOnly);
    Q_ASSERT(videoFrame.mappedBytes(0) == frame->linesize[0] * frame->height);
    std::memcpy(videoFrame.bits(0), frame->data[0], videoFrame.mappedBytes(0));
    videoFrame.unmap();
}

void VideoStream::logAVFrame(void* context, int level, const AVFrame* frame) const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    av_log(context, level, "pict_type %c format: %s\n",
        av_get_picture_type_char(frame->pict_type), av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)));
}

// NOLINTEND(bugprone-assignment-in-if-condition)