#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

CURRENT="$(cd "${BASH_SOURCE%/*}"; pwd)"
source "$CURRENT/../versions"
INSTALLDIR=${QTDIR%/*/*}
(
    cd "$BUILD_ROOT"
    QT_VER_NODOT=${QT_VER//.}
    QTSPEC=qt6.${QT_VER_NODOT}
    INSTALLER=qt-online-installer-macOS-x64-${QT_INSTALLER_VER}
    curl -L -O https://qt.mirror.constant.com/archive/online_installers/${QT_INSTALLER_VER%.*}/${INSTALLER}.dmg \
    && hdiutil attach ${INSTALLER}.dmg \
    && /Volumes/${INSTALLER}/${INSTALLER}.app/Contents/MacOS/${INSTALLER} \
        --root "${INSTALLDIR}" --accept-licenses --accept-obligations --default-answer --confirm-command \
        --auto-answer telemetry-question=No --no-default-installations --no-force-installations \
        install \
        qt.${QTSPEC}.clang_64 \
        qt.${QTSPEC}.addons.qtquick3d \
        qt.${QTSPEC}.addons.qtquicktimeline \
        qt.${QTSPEC}.addons.qtpositioning \
        qt.${QTSPEC}.addons.qtwebchannel \
        extensions.qtwebengine.${QT_VER_NODOT}.clang_64 \
        qt.${QTSPEC}.addons.qtmultimedia \
        qt.${QTSPEC}.addons.qtshadertools \
        qt.${QTSPEC}.addons.qtquickeffectmaker \
        qt.tools.qtcreator_gui \
    && hdiutil detach /Volumes/${INSTALLER} \
    && find "${INSTALLDIR}/Docs/Qt-${QT_VER}" -type f -and -not -name '*.index' -delete
)
