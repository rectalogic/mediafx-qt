#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
#
# This file is part of mediaFX.
#
# mediaFX is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with mediaFX.
# If not, see <https://www.gnu.org/licenses/>.

usage="$0 <mediafxpath> <qsbpath> <framerate>:<WxH> <qml-file> <output-file> [asset-spec ...]"

BASE=${BASH_SOURCE%/*}

FIXTURES=${BASE}/fixtures
ASSETS=${FIXTURES}/assets

MEDIAFX=${1:?$usage}
shift
QSB=${1:?$usage}
shift
IFS=: read -r FRAMERATE SIZE <<< $1
shift
QML=${1:?$usage}
shift
OUTPUT=${1:?$usage}
shift
mkdir -p $(dirname "${OUTPUT}")

for assetspec in "$@"; do
    "${BASE}/../tools/create-asset.sh" "${ASSETS}" ${assetspec} || exit 1
done

for shader in $(find "${BASE}/qml" -name '*.frag' -o -name '*.vert'); do
    if [ "${shader}.qsb" -ot "${shader}" ]; then
        "${QSB}" --glsl "100 es,120,150" --hlsl 50 --msl 12 -o "${shader}.qsb" "${shader}"
    fi
done


echo Testing ${QML}
export QT_LOGGING_RULES="qt.qml.binding.removal.info=true"
"${MEDIAFX}" --fps ${FRAMERATE} --size ${SIZE} --output "${OUTPUT}" --command "ffmpeg:lossless" "${QML}" || exit 1
"${BASE}/../tools/framehash.sh" "${OUTPUT}" > "${OUTPUT}.framehash" || exit 1

# If framehash is different, then pixel difference each frame.
# threshold=2 is how much each pixel can differ
# https://superuser.com/questions/1615310/how-to-use-ffmpeg-blend-difference-filter-mode-to-identify-frame-differences-bet
FIXTURE=${FIXTURES}/$(basename "${OUTPUT}")
[ -f "${FIXTURE}.framehash" ] || exit 1
if ( ! diff "${FIXTURE}.framehash" "${OUTPUT}.framehash" ); then
    echo Warning: framehash is different, comparing frames
    mkdir -p "${OUTPUT}-png"
    ( ! ffmpeg -hide_banner -i "${FIXTURE}" -i "${OUTPUT}" -filter_complex "blend=all_mode=difference,blackframe=amount=0:threshold=3,metadata=select:key=lavfi.blackframe.pblack:value=99.999:function=less,metadata=print:file=-" -an -v 24 "${OUTPUT}-png/frame-%05d.png" | grep pblack ) || exit 1
fi
