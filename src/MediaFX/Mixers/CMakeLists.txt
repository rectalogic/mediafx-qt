# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(mixers STATIC)

qt_add_qml_module(mixers
    URI MediaFX.Mixers
    QML_FILES
    MediaMixer.qml
    CrossFadeMixer.qml
    LumaMixer.qml
    LumaGradientMixer.qml
    WipeMixer.qml
)
qt_add_shaders(mixers "shaders"
    PREFIX
    "/shaders"
    FILES
    crossfade.frag
    luma.frag
)
