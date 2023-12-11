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

CURRENT=$(dirname "${BASH_SOURCE[0]}")
source "$CURRENT/../versions"
INSTALL_ROOT="${BUILD_ROOT:?}/Darwin/installed/${QT_VER:?}/macos"
BUILD_TYPE=${BUILD_TYPE:-Release}

MEDIAFX_ROOT="$(cd "$(dirname "${CURRENT}")/../"; pwd)"

MEDIAFX_BUILD="${BUILD_ROOT}/Darwin/build/mediafx/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"

PATH=${INSTALL_ROOT:?}/bin:$PATH

cd "$MEDIAFX_BUILD"
(cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix "$INSTALL_ROOT" "$MEDIAFX_ROOT" && cmake --build . && cmake --install .) || exit 1
if [[ -v MEDIAFX_TEST ]]; then
    make test CTEST_OUTPUT_ON_FAILURE=1 ARGS="${MEDIAFX_TEST}" || exit 1
fi