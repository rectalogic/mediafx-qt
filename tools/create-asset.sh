#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

# Create a colored "testsrc" video/image asset, with no audio

BASE=${BASH_SOURCE%/*}

usage="$0 <outputdir> <png|mp4>~[[color]~[WxH]~[framerate]~[duration]]"
IFS='~' read -r TYPE COLOR SIZE FRAMERATE DURATION <<< $2

COLOR=${COLOR:-red}
SIZE=${SIZE:-640x360}
FRAMERATE=${FRAMERATE:-30}
DURATION=${DURATION:-12}

mkdir -p "$1"


case ${TYPE:?$usage} in
mp4)
    OUTPUT=${1:?$usage}/${COLOR}-${SIZE}-${FRAMERATE}fps-${DURATION}s.${TYPE}
    if [ ! -f "${OUTPUT}" ]; then
        echo Creating ${OUTPUT}
        ffmpeg -f lavfi -i "testsrc=duration=${DURATION}:size=${SIZE}:rate=${FRAMERATE}:decimals=2,drawbox=color=${COLOR}:t=ih/16,drawtext=text=${COLOR} f%{frame_num}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9:fontfile=${BASE}/JetBrainsMono-Regular.ttf" -f mp4 -vcodec libx264 -preset veryslow -qp 0 -y "${OUTPUT}" || exit 1
    fi
    ;;
png)
    OUTPUT=${1:?$usage}/${COLOR}-${SIZE}.${TYPE}
    if [ ! -f "${OUTPUT}" ]; then
        echo Creating ${OUTPUT}
        ffmpeg -f lavfi -i "color=color=${COLOR}:duration=1:size=${SIZE}:rate=1,drawtext=text=${COLOR}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9:fontfile=${BASE}/JetBrainsMono-Regular.ttf" -r 1 -vframes 1 -c:v png -y "${OUTPUT}" || exit 1
    fi
    ;;
*)
    echo "$0: unknown asset type: $TYPE"
    exit 1
esac