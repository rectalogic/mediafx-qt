#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

# Create a colored "testsrc" video/image asset, with no audio

BASE=${BASH_SOURCE%/*}

usage="$0 <outputdir> <png|nut>~[[color]~[WxH]~[framerate]~[duration]~[voice]~[samplerate]]"
IFS='~' read -r TYPE COLOR SIZE FRAMERATE DURATION VOICE SAMPLERATE <<< $2

COLOR=${COLOR:-red}
SIZE=${SIZE:-640x360}
FRAMERATE=${FRAMERATE:-30}
DURATION=${DURATION:-12}
SAMPLERATE=${SAMPLERATE:-44100}

mkdir -p "$1"

case ${TYPE:?$usage} in
nut)
    # ffmpeg -f lavfi -i flite=list_voices=1
    AUDIO_FILTER="-an"
    if [ "${VOICE}" != "" ]; then
        SPEECH=$(echo "$COLOR "{0..30})
        AUDIO_FILTER_OPTIONS="-f lavfi -t ${DURATION} -i"
        AUDIO_FILTER="flite=text=${SPEECH}:voice=${VOICE}"
        AUDIO_ENCODE_OPTIONS="-codec:a wavpack -ac 2 -ar ${SAMPLERATE}"
        AUDIO_SUFFIX="-${VOICE}${SAMPLERATE}"
    fi
    OUTPUT=${1:?$usage}/${COLOR}-${SIZE}-${FRAMERATE}fps-${DURATION}s${AUDIO_SUFFIX}.${TYPE}
    if [ ! -f "${OUTPUT}" ]; then
        echo Creating ${OUTPUT}
        ffmpeg -hide_banner -f lavfi -i "testsrc=duration=${DURATION}:size=${SIZE}:rate=${FRAMERATE}:decimals=2,drawbox=color=${COLOR}:t=ih/16,drawtext=text=${COLOR} f%{frame_num}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9:fontfile=${BASE}/JetBrainsMono-Regular.ttf" ${AUDIO_FILTER_OPTIONS} "${AUDIO_FILTER}" -f nut ${AUDIO_ENCODE_OPTIONS} -codec:v libx264 -preset veryslow -qp 0 -y "${OUTPUT}" || exit 1
    fi
    ;;
png)
    OUTPUT=${1:?$usage}/${COLOR}-${SIZE}.${TYPE}
    if [ ! -f "${OUTPUT}" ]; then
        echo Creating ${OUTPUT}
        ffmpeg -hide_banner -f lavfi -i "color=color=${COLOR}:duration=1:size=${SIZE}:rate=1,drawtext=text=${COLOR}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9:fontfile=${BASE}/JetBrainsMono-Regular.ttf" -r 1 -vframes 1 -c:v png -y "${OUTPUT}" || exit 1
    fi
    ;;
*)
    echo "$0: unknown asset type: $TYPE"
    exit 1
esac