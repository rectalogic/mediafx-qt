#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

BUILD_TYPE=${BUILD_TYPE:-Debug}
mkdir -p /mediafx/build/Linux/${BUILD_TYPE}
cd /mediafx/build/Linux/${BUILD_TYPE}
if [ ! -f "qt${QT_VER}.imp" ]; then
    curl -O https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/clang_13/mapgen/iwyu-mapgen-qt.py
    python3 iwyu-mapgen-qt.py ${QTDIR}/include > qt${QT_VER}.imp
fi
CC="clang" CXX="clang++" cmake -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/usr/bin/iwyu;-Xiwyu;--mapping_file=/mediafx/build/Linux/${BUILD_TYPE}/qt${QT_VER}.imp;-Xiwyu;--mapping_file=/mediafx/builders/Linux/iwyu.imp;-Xiwyu;--cxx17ns;-Xiwyu;--no_comments" --install-prefix ${QTDIR} ../../.. && exec cmake --build . --parallel 4
