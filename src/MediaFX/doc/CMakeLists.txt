# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 OPTIONAL_COMPONENTS CoreTools Tools)

add_custom_target(
    docs
    "$<TARGET_PROPERTY:Qt6::qdoc,IMPORTED_LOCATION>" "--log-progress" "-indexdir" "${QT6_INSTALL_PREFIX}/../../Docs/Qt-${Qt6_VERSION}" "-outputdir" "${CMAKE_BINARY_DIR}/doc" "${CMAKE_CURRENT_SOURCE_DIR}/mediafx.qdocconf"
    VERBATIM
)
