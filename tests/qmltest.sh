#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

usage="$0 <mediafxpath> <framerate>:<WxH> <qml-file> <output-file> <threshold>"

BASE=${BASH_SOURCE%/*}

FIXTURES=${BASE}/fixtures
ASSETS=${FIXTURES}/assets

MEDIAFX=${1:?$usage}
shift
IFS=: read -r FRAMERATE SIZE <<< $1
shift
QML=${1:?$usage}
shift
OUTPUT=${1:?$usage}
shift
mkdir -p $(dirname "${OUTPUT}")
THRESHOLD=${1:?$usage}
shift

echo Testing ${QML}
export QT_LOGGING_RULES="qt.qml.binding.removal.info=true"
"${MEDIAFX}" --exitOnWarning --fps ${FRAMERATE} --size ${SIZE} --output "${OUTPUT}" --command "ffmpeg:lossless" "${QML}" || exit 1

"${BASE}/../tools/framehash.sh" "${OUTPUT}" > "${OUTPUT}.framehash" || exit 1

function stream_hashes {
    local file=$1
    local media=$2
    local stream_index=$(sed -n "s/^#media_type \(.*\): ${media}/\1/gp" "$file")
    grep "^${stream_index}, " "$file"
}

# If framehash is different, then pixel difference each frame.
# threshold=2 is how much each pixel can differ
# https://superuser.com/questions/1615310/how-to-use-ffmpeg-blend-difference-filter-mode-to-identify-frame-differences-bet
FIXTURE=${FIXTURES}/$(basename "${OUTPUT}")
[ -f "${FIXTURE}.framehash" ] || exit 1

diff <(stream_hashes "${FIXTURE}.framehash" audio) <(stream_hashes "${OUTPUT}.framehash" audio)
AUDIO_COMPARE=$?
if (( $AUDIO_COMPARE )); then
    echo Warning: audio framehash is different, comparing frames
    # Invert one audio and mix with the other. Result should be silence if identical.
    # Extract max volume dB and ensure it is very quiet.
    AUDIO_MAX_VOLUME=$(ffmpeg -hide_banner -vn -i "${FIXTURE}" -vn -i "${OUTPUT}" -filter_complex "[1] aeval=-val(0)|-val(1) [invert]; [0][invert] amix=normalize=0 [out]; [out] volumedetect" -f null - 2>&1 | sed -n "s/.* max_volume: \(.*\) dB/\1/p")
    echo Audio max volume: $AUDIO_MAX_VOLUME dB
    AUDIO_COMPARE=$( bc -l <<< "! ($AUDIO_MAX_VOLUME <= -90)" ) || exit 1
fi

diff <(stream_hashes "${FIXTURE}.framehash" video) <(stream_hashes "${OUTPUT}.framehash" video)
VIDEO_COMPARE=$?
if (( $VIDEO_COMPARE )); then
    echo Warning: video framehash is different, comparing frames
    # https://superuser.com/questions/1615310/how-to-use-ffmpeg-blend-difference-filter-mode-to-identify-frame-differences-bet
    VIDEO_OUTPUT=$( ffmpeg -hide_banner -an -i "${FIXTURE}" -an -i "${OUTPUT}" -filter_complex "blend=all_mode=difference,blackframe=amount=0:threshold=3,metadata=select:key=lavfi.blackframe.pblack:value=${THRESHOLD}:function=less,metadata=print:file=-" -an -loglevel warning -f null - | { grep pblack || true; } ) || exit 1
    [ -z "$VIDEO_OUTPUT" ]
    VIDEO_COMPARE=$?
fi

if (( $AUDIO_COMPARE || $VIDEO_COMPARE )); then
    echo Audio failed: $AUDIO_COMPARE
    echo Video failed: $VIDEO_COMPARE
    "${BASE}/../tools/diff-video.sh" "${FIXTURE}" "${OUTPUT}" "${OUTPUT}.diff.nut"
    echo See "${OUTPUT}.diff.nut"
    exit 1
fi