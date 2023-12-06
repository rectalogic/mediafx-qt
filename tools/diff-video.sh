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
usage="$0 <original-video> <new-video>"

# https://superuser.com/questions/854543/how-to-compare-the-difference-between-2-videos-color-in-ffmpeg
ffplay -autoexit -f lavfi "movie='${1:?$usage}',split[org1][org2]; movie='${2:?$usage}',split[new1][new2]; [org1][new1]blend=all_mode=grainextract[blend];[org2][new2][blend]vstack=inputs=3"