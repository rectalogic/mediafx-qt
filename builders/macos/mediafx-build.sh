#!/usr/bin/env bash

set -u

CURRENT=$(dirname "${BASH_SOURCE[0]}")
source "$CURRENT/versions"

MEDIAFX_ROOT="$(cd "$(dirname "${CURRENT}")/../"; pwd)"

MEDIAFX_BUILD="${BUILD_ROOT}/build/mediafx"
mkdir -p "$MEDIAFX_BUILD"

PATH=$INSTALL_ROOT/bin:$PATH

cd "$MEDIAFX_BUILD"
cmake --install-prefix "$INSTALL_ROOT" "$MEDIAFX_ROOT" && cmake --build . && cmake --install .
