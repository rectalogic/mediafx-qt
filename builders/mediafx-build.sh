#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/versions"

BUILD_TYPE=${BUILD_TYPE:-Release}
MEDIAFX_BUILD="${BUILD_ROOT}/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"
(cmake -S "${SOURCE_ROOT}" -B "$MEDIAFX_BUILD" -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix "${QTDIR}" && cmake --build "$MEDIAFX_BUILD" && ${SUDO} cmake --install "$MEDIAFX_BUILD") || exit 1
