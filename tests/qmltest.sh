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

usage="$0 <mediafxpath> <framerate>:<WxH> <qml-file> <output-file> [asset-spec ...]"

BASE=$(dirname "${BASH_SOURCE[0]}")

ASSETS=${BASE}/../build/assets
FIXTURES=${BASE}/fixtures/$(uname)

MEDIAFX=${1:?$usage}
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

FFMPEG='ffmpeg -f rawvideo -video_size ${MEDIAFX_FRAMESIZE} -pixel_format rgb0 -framerate ${MEDIAFX_FRAMERATE} -i pipe:${MEDIAFX_VIDEOFD} -f nut -vcodec ffv1 -flags bitexact -g 1 -level 3 -pix_fmt rgb32 -framerate ${MEDIAFX_FRAMERATE} -fflags bitexact -y ${MEDIAFX_OUTPUT}'

echo Testing ${QML}
"${MEDIAFX}" --fps ${FRAMERATE} --size ${SIZE} --output "${OUTPUT}" --command "${FFMPEG}" "${QML}" || exit 1
"${BASE}/../tools/framehash.sh" "${OUTPUT}" > "${OUTPUT}.framehash" || exit 1

diff "${FIXTURES}/$(basename "${OUTPUT}").framehash" "${OUTPUT}.framehash" || exit 1
