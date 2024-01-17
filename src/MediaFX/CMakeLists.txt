# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(PkgConfig)
pkg_search_module(ffms2 REQUIRED IMPORTED_TARGET ffms2)
pkg_search_module(libavutil REQUIRED IMPORTED_TARGET libavutil)

option(EVENT_LOGGER "Enable event logger" OFF)

qt_add_library(mediafx STATIC
    application.cpp
    session.cpp
    media_manager.cpp
    encoder.cpp
    render_control.cpp
    media_clip.cpp
    track.cpp
    video_track.cpp
    audio_track.cpp
    media.cpp
    interval.cpp
)

target_include_directories(mediafx PUBLIC ./)
target_include_directories(mediafx PRIVATE ${FFMS2_INCLUDE_DIRS} ${LIBAVUTIL_INCLUDE_DIRS})
target_compile_options(mediafx PUBLIC ${FFMS2_CFLAGS})

qt_add_executable(mediafxtool
    main.cpp
)

if(EVENT_LOGGER)
    target_sources(mediafxtool PRIVATE event_logger.h)
    add_compile_definitions(EVENTLOGGER)
endif()

set_property(TARGET mediafxtool PROPERTY OUTPUT_NAME mediafx)

qt_add_qml_module(mediafx
    URI MediaFX
    QML_FILES
    qml/sequence.js
    qml/MediaSequence.qml
    qml/MultiEffectState.qml
    qml/ShaderEffectState.qml
    qml/mixers/MediaMixer.qml
    qml/mixers/CrossFadeMixer.qml
    qml/mixers/LumaMixer.qml
    qml/mixers/LumaGradientMixer.qml
    qml/mixers/WipeMixer.qml
)
qt_add_shaders(mediafx "shaders"
    PREFIX
    "/"
    FILES
    qml/mixers/crossfade.frag
    qml/mixers/luma.frag
)

target_link_libraries(mediafx PUBLIC PkgConfig::ffms2 Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::Multimedia Qt6::Qml Qt6::Quick)

if(${Qt6Quick3D_FOUND})
    target_link_libraries(mediafx PUBLIC Qt6::Quick3D)
endif()

if(${Qt6WebEngineQuick_FOUND})
    target_link_libraries(mediafx PUBLIC Qt6::WebEngineQuick)
    add_compile_definitions(WEBENGINEQUICK)
endif()

target_link_libraries(mediafxtool PRIVATE mediafx mediafxplugin)

install(TARGETS mediafxtool RUNTIME DESTINATION)

add_subdirectory(doc)