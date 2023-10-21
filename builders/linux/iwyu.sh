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

set -e

sudo mkdir -p /mediafx/build/linux && sudo chown mediafx /mediafx/build/ /mediafx/build/linux
cd /mediafx/build/linux
if [ ! -f "qt${QT_VER}.imp" ]; then
    curl -O https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/clang_13/mapgen/iwyu-mapgen-qt.py
    python3 iwyu-mapgen-qt.py /usr/local/Qt/${QT_VER}/gcc_64/include > qt${QT_VER}.imp
fi
CC="clang-13" CXX="clang++-13" cmake -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/usr/bin/iwyu;-Xiwyu;--mapping_file=/mediafx/build/linux/qt${QT_VER}.imp;-Xiwyu;--cxx17ns;-Xiwyu;--no_comments" --install-prefix /usr/local/Qt/${QT_VER}/gcc_64 ../.. && cmake --build .
