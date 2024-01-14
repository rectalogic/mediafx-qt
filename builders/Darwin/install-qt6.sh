#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"
INSTALLDIR=${QTDIR%/*/*}
(
    cd "$BUILD_ROOT"
    python3 -m venv --clear "qtvenv" || exit 1
    "qtvenv/bin/pip" install --upgrade --upgrade-strategy eager aqtinstall || exit 1
    "qtvenv/bin/python" -m aqt install-qt mac desktop ${QT_VER} --modules qtmultimedia qtquick3d qtshadertools qtquicktimeline qtquickeffectmaker -O "${INSTALLDIR}" || exit 1
    "qtvenv/bin/python" -m aqt install-tool mac desktop tools_qtcreator_gui qt.tools.qtcreator_gui -O "${QTDIR}/bin" || exit 1
    "qtvenv/bin/python" -m aqt install-src mac ${QT_VER} --archives qtbase qtdeclarative qtmultimedia qtquicktimeline -O "${INSTALLDIR}" || exit 1
)
