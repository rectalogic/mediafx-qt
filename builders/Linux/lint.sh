#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"

BUILD_TYPE=${BUILD_TYPE:-Debug}
MEDIAFX_BUILD=${BUILD_ROOT}/${BUILD_TYPE}
mkdir -p "${MEDIAFX_BUILD}"
cmake -S "${SOURCE_ROOT}" -B "$MEDIAFX_BUILD" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} --install-prefix ${QTDIR} || exit 1
# Generate *.moc include files for tests
cmake --build "${MEDIAFX_BUILD}" --target tst_encoder_autogen/fast tst_decoder_autogen/fast tst_interval_autogen/fast || exit 1

cd /mediafx
git config --global --add safe.directory /mediafx
FILES=$(git ls-files '**/*.cpp' '**/*.h')
/usr/bin/clang-tidy -p "${MEDIAFX_BUILD}" $FILES || exit 1
/usr/bin/clazy-standalone -p "${MEDIAFX_BUILD}/compile_commands.json" $FILES || exit 1