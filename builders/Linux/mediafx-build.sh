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

cd "$(dirname "${BASH_SOURCE[0]}")/../../build/Linux" || exit 1
(cmake --install-prefix /usr/local/Qt/${QT_VER:?}/gcc_64 ../.. && cmake --build . && sudo cmake --install .) || exit 1
if [[ -v MEDIAFX_TEST ]]; then
    make test CTEST_OUTPUT_ON_FAILURE=1 ARGS="${MEDIAFX_TEST}" || exit 1
fi