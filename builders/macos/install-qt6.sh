#!/usr/bin/env bash

set -u

CURRENT=$(dirname "${BASH_SOURCE[0]}")
source "$CURRENT/versions"
(
    cd "$BUILD_ROOT"
    python3 -m venv --clear "build/qtvenv"
    "build/qtvenv/bin/pip" install --upgrade --upgrade-strategy eager aqtinstall
    "build/qtvenv/bin/python" -m aqt install-qt mac desktop ${QT_VER} --modules qtmultimedia qtquick3d qtshadertools qtquicktimeline -O "$BUILD_ROOT/installed"
)
