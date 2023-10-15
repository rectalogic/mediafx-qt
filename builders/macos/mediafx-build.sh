#!/usr/bin/env bash

set -u

BUILD_TYPE=${BUILD_TYPE:-Release}
CURRENT=$(dirname "${BASH_SOURCE[0]}")
source "$CURRENT/versions"

MEDIAFX_ROOT="$(cd "$(dirname "${CURRENT}")/../"; pwd)"

MEDIAFX_BUILD="${BUILD_ROOT}/build/mediafx/${BUILD_TYPE}"
mkdir -p "$MEDIAFX_BUILD"

PATH=$INSTALL_ROOT/bin:$PATH

cd "$MEDIAFX_BUILD"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE --install-prefix "$INSTALL_ROOT" "$MEDIAFX_ROOT" && cmake --build . && cmake --install .
