#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

mkdir -p /mediafx/build/Linux
cd /mediafx/build/Linux
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --install-prefix ${QTDIR} ../..
# Generate *.moc include files for tests
make tst_encoder_autogen/fast tst_interval_autogen/fast 
cd /mediafx
FILES=$(git ls-files '**/*.cpp' '**/*.h')
/usr/bin/clang-tidy-15 -p /mediafx/build/Linux $FILES
/usr/bin/clazy-standalone -p /mediafx/build/Linux/compile_commands.json $FILES