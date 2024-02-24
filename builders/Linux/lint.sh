#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

BUILD_TYPE=${BUILD_TYPE:-Debug}
mkdir -p /mediafx/build/Linux/${BUILD_TYPE}
cd /mediafx/build/Linux/${BUILD_TYPE}
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} --install-prefix ${QTDIR} ../../.. || exit 1
# Generate *.moc include files for tests
make tst_encoder_autogen/fast tst_decoder_autogen/fast tst_interval_autogen/fast || exit 1
cd /mediafx
git config --global --add safe.directory /mediafx
FILES=$(git ls-files '**/*.cpp' '**/*.h')
/usr/bin/clang-tidy -p /mediafx/build/Linux/${BUILD_TYPE} $FILES || exit 1
/usr/bin/clazy-standalone -p /mediafx/build/Linux/${BUILD_TYPE}/compile_commands.json $FILES || exit 1