#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

ffmpeg -loglevel error -i ${1:?video file required} -c copy -f hash -hash md5 -
