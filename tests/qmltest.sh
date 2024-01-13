#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

usage="$0 <mediafxpath> <framerate>:<WxH> <qml-file> <output-file> <threshold> [asset-spec ...]"

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
THRESHOLD=${1:?$usage}
shift
mkdir -p $(dirname "${OUTPUT}")

for assetspec in "$@"; do
    "${BASE}/../tools/create-asset.sh" "${ASSETS}" ${assetspec} || exit 1
done

echo Testing ${QML}
export QT_LOGGING_RULES="qt.qml.binding.removal.info=true"
"${MEDIAFX}" --exitOnWarning --fps ${FRAMERATE} --size ${SIZE} --output "${OUTPUT}" --command "ffmpeg:lossless" "${QML}" || exit 1
"${BASE}/../tools/framehash.sh" "${OUTPUT}" > "${OUTPUT}.framehash" || exit 1

# If framehash is different, then pixel difference each frame.
# threshold=2 is how much each pixel can differ
# https://superuser.com/questions/1615310/how-to-use-ffmpeg-blend-difference-filter-mode-to-identify-frame-differences-bet
FIXTURE=${FIXTURES}/$(basename "${OUTPUT}")
[ -f "${FIXTURE}.framehash" ] || exit 1
if ( ! diff "${FIXTURE}.framehash" "${OUTPUT}.framehash" ); then
    echo Warning: framehash is different, comparing frames
    mkdir -p "${OUTPUT}-png"
    ( ! ffmpeg -hide_banner -i "${FIXTURE}" -i "${OUTPUT}" -filter_complex "blend=all_mode=difference,blackframe=amount=0:threshold=3,metadata=select:key=lavfi.blackframe.pblack:value=${THRESHOLD}:function=less,metadata=print:file=-" -an -v 24 "${OUTPUT}-png/frame-%05d.png" | grep pblack ) || exit 1
fi
