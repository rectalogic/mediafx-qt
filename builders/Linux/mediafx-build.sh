#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"
BUILD_TYPE=${BUILD_TYPE:-Release}
MEDIAFX_BUILD="${BUILD_ROOT:?}/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"
cd "$MEDIAFX_BUILD"
(cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix /usr/local/Qt/${QT_VER:?}/gcc_64 ../../.. && cmake --build . && sudo cmake --install .) || exit 1
if [[ -v MEDIAFX_TEST ]]; then
    export GALLIUM_DRIVER=softpipe
    export LIBGL_ALWAYS_SOFTWARE=1
    export DRI_NO_MSAA=1
    make test CTEST_OUTPUT_ON_FAILURE=1 ARGS="${MEDIAFX_TEST}" || exit 1
fi