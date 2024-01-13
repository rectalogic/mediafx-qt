#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"
INSTALL_ROOT="${BUILD_ROOT:?}/installed/${QT_VER:?}/macos"
BUILD_TYPE=${BUILD_TYPE:-Release}

MEDIAFX_ROOT="$(cd "$(dirname "${CURRENT}")/../"; pwd)"

MEDIAFX_BUILD="${BUILD_ROOT}/build/mediafx/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"

PATH=${INSTALL_ROOT:?}/bin:$PATH

cd "$MEDIAFX_BUILD"
(cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix "$INSTALL_ROOT" "$MEDIAFX_ROOT" && cmake --build . && cmake --install .) || exit 1
if [[ -v MEDIAFX_TEST ]]; then
    make test CTEST_OUTPUT_ON_FAILURE=1 ARGS="${MEDIAFX_TEST}" || exit 1
fi