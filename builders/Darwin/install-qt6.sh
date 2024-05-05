#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

CURRENT="$(cd "${BASH_SOURCE%/*}"; pwd)"
source "$CURRENT/../versions"
INSTALLDIR=${QTDIR%/*/*}
(
    cd "$BUILD_ROOT"
    QTSPEC=qt6.${QT_VER//.}
    curl -O https://qt.mirror.constant.com/archive/online_installers/${QT_INSTALLER_VER%.*}/qt-unified-macOS-x64-${QT_INSTALLER_VER}-online.dmg \
    && hdiutil attach qt-unified-macOS-x64-${QT_INSTALLER_VER}-online.dmg \
    && /Volumes/qt-unified-macOS-x64-${QT_INSTALLER_VER}-online/qt-unified-macOS-x64-${QT_INSTALLER_VER}-online.app/Contents/MacOS/qt-unified-macOS-x64-${QT_INSTALLER_VER}-online \
        --root "${INSTALLDIR}" --accept-licenses --accept-obligations --default-answer --confirm-command \
        --auto-answer telemetry-question=No --no-default-installations --no-force-installations \
        install \
        qt.${QTSPEC}.clang_64 \
        qt.${QTSPEC}.qtquick3d \
        qt.${QTSPEC}.qtquicktimeline \
        qt.${QTSPEC}.addons.qtpositioning \
        qt.${QTSPEC}.addons.qtwebchannel \
        qt.${QTSPEC}.addons.qtwebengine \
        qt.${QTSPEC}.addons.qtmultimedia \
        qt.${QTSPEC}.qtshadertools \
        qt.${QTSPEC}.addons.qtquickeffectmaker \
        qt.tools.qtcreator_gui \
    && hdiutil detach /Volumes/qt-unified-macOS-x64-${QT_INSTALLER_VER}-online \
    && find "${INSTALLDIR}/Docs/Qt-${QT_VER}" -type f -and -not -name '*.index' -delete
)
