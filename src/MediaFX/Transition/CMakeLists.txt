# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(transition STATIC)

foreach(shader_output_target ${gltransitionshader_output_targets})
    add_dependencies(transition ${shader_output_target})
endforeach()

# https://bugreports.qt.io/browse/QTBUG-103723
qt_add_shaders(transition "shaders"
    PREFIX
    "/shaders"
    FILES
    crossfade.frag
    luma.frag
    pageCurl.frag
    OUTPUT_TARGETS shader_output_targets
)

foreach(shader_output_target ${shader_output_targets})
    add_dependencies(transition ${shader_output_target})
endforeach()

qt_add_qml_module(transition
    URI MediaFX.Transition
    QML_FILES
    MediaTransition.qml
    TransitionShaderEffect.qml
    CrossFade.qml
    Luma.qml
    LumaGradient.qml
    PageCurl.qml
    Wipe.qml
)

add_subdirectory(GL)