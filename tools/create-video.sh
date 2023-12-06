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

ROOT=$(dirname "${BASH_SOURCE[0]}")

ffmpeg -f lavfi -i testsrc=duration=${DURATION:-12}:size=${SIZE:-640x360}:rate=${FRAMERATE:-30}:decimals=2,drawbox=color=${COLOR:-red}:t=ih/16,drawtext=text=f%{frame_num}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9:fontfile=${ROOT}/JetBrainsMono-Regular.ttf -f nut -vcodec ffv1 -g 1 -level 3 -pix_fmt rgb32 -framerate ${FRAMERATE:-30}  -y ${1:?specify output nut filename}