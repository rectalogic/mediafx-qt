#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later
usage="$0 <original-video> <new-video> <output.nut>"

# https://superuser.com/questions/854543/how-to-compare-the-difference-between-2-videos-color-in-ffmpeg
ffmpeg -f lavfi -i "movie='${1:?$usage}',split[org1][org2]; movie='${2:?$usage}',split[new1][new2]; [org1][new1]blend=all_mode=grainextract[blend];[org2][new2][blend]vstack=inputs=3" -f nut -vcodec ffv1 -flags bitexact -g 1 -level 3 -pix_fmt rgb32 -fflags bitexact -y "${3:?$usage}"