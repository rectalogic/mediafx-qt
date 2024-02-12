# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(PkgConfig)
pkg_search_module(ffms2 REQUIRED IMPORTED_TARGET ffms2)
pkg_search_module(libavformat REQUIRED IMPORTED_TARGET libavformat)
pkg_search_module(libavcodec REQUIRED IMPORTED_TARGET libavcodec)
pkg_search_module(libavutil REQUIRED IMPORTED_TARGET libavutil)
pkg_search_module(libswresample REQUIRED IMPORTED_TARGET libswresample)

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
    audio_renderer.cpp
    interval.cpp
)

target_include_directories(mediafx PUBLIC ./)
target_include_directories(mediafx PRIVATE ${FFMS2_INCLUDE_DIRS} ${LIBAVFORMAT_INCLUDE_DIRS} ${LIBAVCODEC_INCLUDE_DIRS} ${LIBAVUTIL_INCLUDE_DIRS} ${LIBSWRESAMPLE_INCLUDE_DIRS})
target_compile_options(mediafx PUBLIC ${FFMS2_CFLAGS} ${LIBAVFORMAT_CFLAGS} ${LIBAVCODEC_CFLAGS} ${LIBAVUTIL_CFLAGS} ${LIBSWRESAMPLE_CFLAGS})

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
    VideoRenderer.qml
    sequence.js
    MediaSequence.qml
    MultiEffectState.qml
    ShaderEffectState.qml
)

target_link_libraries(mediafx PUBLIC PkgConfig::ffms2 PkgConfig::libswresample PkgConfig::libavformat PkgConfig::libavcodec PkgConfig::libavutil Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::Multimedia Qt6::Qml Qt6::Quick)
target_link_libraries(mediafx PRIVATE mediafxplugin mixersplugin)

if(${Qt6Quick3D_FOUND})
    target_link_libraries(mediafx PUBLIC Qt6::Quick3D)
endif()

if(${Qt6WebEngineQuick_FOUND})
    target_link_libraries(mediafx PUBLIC Qt6::WebEngineQuick)
    add_compile_definitions(WEBENGINEQUICK)
endif()

target_link_libraries(mediafxtool PRIVATE mediafx)

install(TARGETS mediafxtool RUNTIME DESTINATION)

add_subdirectory(Mixers)
add_subdirectory(doc)