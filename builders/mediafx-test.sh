#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/versions"

BUILD_TYPE=${BUILD_TYPE:-Release}
cd "${BUILD_ROOT}/${BUILD_TYPE}" || exit 1
ctest --output-on-failure "${@}" || exit 1
