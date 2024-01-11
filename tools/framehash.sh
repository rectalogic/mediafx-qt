#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

ffmpeg -loglevel error -i ${1:?specify input video} -f framehash -hash md5 -fflags bitexact -
