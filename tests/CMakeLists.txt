# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

macro(add_compile_shaders)
    cmake_parse_arguments(QSB_COMMAND "" "" "SHADERS" ${ARGN})

    foreach(SHADER ${QSB_COMMAND_SHADERS})
        add_custom_command(
            OUTPUT ${SHADER}.qsb
            DEPENDS ${SHADER}
            COMMAND $<TARGET_PROPERTY:Qt::qsb,IMPORTED_LOCATION> --glsl "100 es,120,150" --hlsl 50 --msl 12 -o "${SHADER}.qsb" "${SHADER}"
            VERBATIM
        )
    endforeach()

    list(TRANSFORM QSB_COMMAND_SHADERS APPEND ".qsb")
    add_custom_target(shaders DEPENDS ${QSB_COMMAND_SHADERS})
endmacro()

macro(add_qml_test)
    cmake_parse_arguments(QML_TEST "" "NAME;OUTPUTSPEC;QMLFILE;OUTPUTFILE;THRESHOLD" "" ${ARGN})
    add_test(NAME ${QML_TEST_NAME} COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/qmltest.sh
        $<TARGET_FILE:mediafxtool>
        ${QML_TEST_OUTPUTSPEC}
        ${CMAKE_CURRENT_SOURCE_DIR}/qml/${QML_TEST_QMLFILE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/${CMAKE_SYSTEM_NAME}/output/${QML_TEST_OUTPUTFILE}
        ${QML_TEST_THRESHOLD}
    )
    set_tests_properties(${QML_TEST_NAME} PROPERTIES DEPENDS "tst_shaders")
endmacro()

add_compile_shaders(SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/qml/grayscale.frag)

# This test only exists as a DEPENDS for each test to trigger compiling shaders before running tests
qt_add_executable(tst_shaders tst_shaders.cpp)
add_test(NAME tst_shaders COMMAND tst_shaders)
add_dependencies(tst_shaders shaders)

qt_add_executable(tst_interval tst_interval.cpp)
add_test(NAME tst_interval COMMAND tst_interval)
target_link_libraries(tst_interval PRIVATE mediafx Qt::Test)

qt_add_executable(tst_encoder tst_encoder.cpp)
add_test(NAME tst_encoder COMMAND tst_encoder)
target_link_libraries(tst_encoder PRIVATE mediafx Qt::Test)

add_qml_test(NAME tst_qml_static OUTPUTSPEC 15:320x180 QMLFILE static.qml OUTPUTFILE static.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_animated OUTPUTSPEC 15:320x180 QMLFILE animated.qml OUTPUTFILE animated.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_video_clipstart OUTPUTSPEC 15:320x180 QMLFILE video-clipstart.qml OUTPUTFILE video-clipstart.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_multisink OUTPUTSPEC 30:640x360 QMLFILE multisink.qml OUTPUTFILE multisink.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_video_ad_insertion OUTPUTSPEC 30:320x180 QMLFILE video-ad-insertion.qml OUTPUTFILE video-ad-insertion.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_video_multieffect OUTPUTSPEC 30:320x180 QMLFILE video-multieffect.qml OUTPUTFILE video-multieffect.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_video_shadereffect OUTPUTSPEC 30:320x180 QMLFILE video-shadereffect.qml OUTPUTFILE video-shadereffect.nut THRESHOLD 99.999)
add_qml_test(NAME tst_qml_sequence OUTPUTSPEC 15:320x180 QMLFILE sequence.qml OUTPUTFILE sequence.nut THRESHOLD 98.999)
add_qml_test(NAME tst_qml_demo OUTPUTSPEC 15:320x180 QMLFILE demo.qml OUTPUTFILE demo.nut THRESHOLD 99.999)

# Label tests that require a GPU
set_tests_properties(tst_qml_static tst_qml_animated tst_qml_video_clipstart tst_qml_multisink tst_qml_video_ad_insertion tst_qml_video_multieffect tst_qml_video_shadereffect tst_qml_sequence PROPERTIES LABELS GPU)