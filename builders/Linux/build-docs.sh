#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

mkdir -p /mediafx/build/Linux
cd /mediafx/build/Linux
cmake --install-prefix ${QTDIR} ../.. && make docs