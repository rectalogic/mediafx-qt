#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"

BUILD_TYPE=${BUILD_TYPE:-Debug}
MEDIAFX_BUILD=${BUILD_ROOT}/${BUILD_TYPE}
mkdir -p "${MEDIAFX_BUILD}"
if [ ! -f "${MEDIAFX_BUILD}/qt${QT_VER}.imp" ]; then
    curl -o "${MEDIAFX_BUILD}/iwyu-mapgen-qt.py" https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/clang_13/mapgen/iwyu-mapgen-qt.py
    python3 "${MEDIAFX_BUILD}/iwyu-mapgen-qt.py" ${QTDIR}/include > "${MEDIAFX_BUILD}/qt${QT_VER}.imp"
fi
CC="clang" CXX="clang++" cmake -S "${SOURCE_ROOT}" -B "$MEDIAFX_BUILD" -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/usr/bin/iwyu;-Xiwyu;--mapping_file=${MEDIAFX_BUILD}/qt${QT_VER}.imp;-Xiwyu;--mapping_file=/mediafx/builders/Linux/iwyu.imp;-Xiwyu;--cxx17ns;-Xiwyu;--no_comments" --install-prefix ${QTDIR} && exec cmake --build "${MEDIAFX_BUILD}" --parallel 4
