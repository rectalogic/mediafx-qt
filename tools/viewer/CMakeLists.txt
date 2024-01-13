# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(mediafxviewer
    main.cpp
)

target_link_libraries(mediafxviewer PUBLIC mediafx mediafxplugin)

qt_add_qml_module(mediafxviewer
    URI MediaFX.Viewer
    QML_FILES
    qml/MediaMixerViewer.qml
)

install(TARGETS mediafxviewer RUNTIME DESTINATION)
