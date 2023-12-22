# Copyright (C) 2023 Andrew Wason
#
# This file is part of mediaFX.
#
# mediaFX is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with mediaFX.
# If not, see <https://www.gnu.org/licenses/>.

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

qt_add_executable(tst_interval tst_interval.cpp)
add_test(NAME tst_interval COMMAND tst_interval)
target_link_libraries(tst_interval PRIVATE mediafx Qt::Test)

qt_add_executable(tst_encoder tst_encoder.cpp)
add_test(NAME tst_encoder COMMAND tst_encoder)
target_link_libraries(tst_encoder PRIVATE mediafx Qt::Test)

macro(add_qml_test NAME OUTPUTSPEC QMLFILE OUTPUTFILE)
    set(ASSETSPECS ${ARGN})
    add_test(NAME ${NAME} COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/qmltest.sh
        $<TARGET_FILE:mediafxtool>
        ${OUTPUTSPEC}
        ${CMAKE_CURRENT_SOURCE_DIR}/qml/${QMLFILE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/${CMAKE_SYSTEM_NAME}/output/${OUTPUTFILE}
        ${ASSETSPECS}
    )
endmacro()

add_qml_test(tst_qml_static 15:320x180 static.qml static.nut)
add_qml_test(tst_qml_animated 15:320x180 animated.qml animated.nut)
add_qml_test(tst_qml_video_clipstart 15:320x180 video-clipstart.qml video-clipstart.nut nut:red:320x180:15:8)
add_qml_test(tst_qml_multisink 30:640x360 multisink.qml multisink.nut nut:red:640x360:30:4 png:red:160x120)
add_qml_test(tst_qml_video_ad_insertion 30:320x180 video-ad-insertion.qml video-ad-insertion.nut nut:red:320x180:15:8 nut:blue:320x180:30:3)

# Label tests that require a GPU
set_tests_properties(tst_qml_static tst_qml_animated tst_qml_video_clipstart tst_qml_multisink tst_qml_video_ad_insertion PROPERTIES LABELS GPU)