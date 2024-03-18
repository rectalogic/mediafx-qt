#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/versions"

BUILD_TYPE=${BUILD_TYPE:-Release}
MEDIAFX_BUILD="${BUILD_ROOT}/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"
cd "$MEDIAFX_BUILD"
(cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix "${QTDIR}" ../../.. && cmake --build . && ${SUDO} cmake --install .) || exit 1
