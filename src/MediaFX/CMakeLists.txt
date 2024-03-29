# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(PkgConfig)
pkg_search_module(libavformat REQUIRED IMPORTED_TARGET libavformat>=58.76.100)
pkg_search_module(libavcodec REQUIRED IMPORTED_TARGET libavcodec>=58.134.100)
pkg_search_module(libavfilter REQUIRED IMPORTED_TARGET libavfilter>=7.110.100)
pkg_search_module(libavutil REQUIRED IMPORTED_TARGET libavutil>=56.70.100)

option(EVENT_LOGGER "Enable event logger" OFF)

find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE MEDIAFX_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(MEDIAFX_VERSION dev)
endif()

configure_file("version.h.in" "version.h")

qt_add_library(mediafx STATIC
    application.cpp
    session.cpp
    media_manager.cpp
    decoder.cpp
    stream.cpp
    audio_stream.cpp
    video_stream.cpp
    encoder.cpp
    output_stream.cpp
    render_control.cpp
    media_clip.cpp
    audio_renderer.cpp
    interval.cpp
)

target_include_directories(mediafx PUBLIC ./)
target_include_directories(mediafx PRIVATE ${LIBAVFORMAT_INCLUDE_DIRS} ${LIBAVCODEC_INCLUDE_DIRS} ${LIBAVFILTER_INCLUDE_DIRS})
target_include_directories(mediafx PUBLIC ${LIBAVUTIL_INCLUDE_DIRS})
target_compile_options(mediafx PRIVATE ${LIBAVFORMAT_CFLAGS} ${LIBAVCODEC_CFLAGS} ${LIBAVFILTER_CFLAGS} ${LIBAVUTIL_CFLAGS})

qt_add_executable(mediafxtool
    main.cpp
)

# For generated version.h
target_include_directories(mediafxtool PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

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

target_link_libraries(mediafx PUBLIC PkgConfig::libavformat PkgConfig::libavcodec PkgConfig::libavfilter PkgConfig::libavutil Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::Multimedia Qt6::Qml Qt6::Quick)
target_link_libraries(mediafx PRIVATE mediafxplugin transitionplugin viewerplugin)

if(${Qt6WebEngineQuick_FOUND})
    target_link_libraries(mediafx PUBLIC Qt6::WebEngineQuick)
    add_compile_definitions(WEBENGINEQUICK)
endif()

target_link_libraries(mediafxtool PRIVATE mediafx)

install(TARGETS mediafxtool RUNTIME DESTINATION)

add_subdirectory(Transition)
add_subdirectory(Viewer)
add_subdirectory(doc)