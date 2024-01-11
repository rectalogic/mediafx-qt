#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

# Transcode a segment of a video, with timestamps overlayed

BASE=${BASH_SOURCE%/*}

usage="$0 <sourcevideo> <outputvideo> <filters> <framerate> <starttime> <endtime>"
SOURCE=${1:?$usage}
OUTPUT=${2:?$usage}
FILTERS=${3:?$usage}
FRAMERATE=${4:?$usage}
STARTTIME=${5:?$usage}
ENDTIME=${6:?$usage}

printf -v TEXT "'%s\n%s'" "f%{frame_num}" "t%{pts\:hms}"
ffmpeg -ss ${STARTTIME} -to ${ENDTIME} -i "${SOURCE}" -vf "${FILTERS},framerate=${FRAMERATE},drawtext=text=${TEXT}:x=0:y=x/dar:shadowx=3:shadowy=3:fontsize=h/6:fontcolor=yellow@0.9:fontfile=${BASE}/JetBrainsMono-Regular.ttf" -f mp4 -vcodec libx264 -preset veryslow -qp 0 -codec:a aac -ac 2 -y "${OUTPUT}" || exit 1