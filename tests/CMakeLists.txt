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

qt_add_executable(interval tst_interval.cpp)
add_test(NAME interval COMMAND interval)
target_link_libraries(interval PRIVATE mediafx Qt::Test)

qt_add_executable(encoder tst_encoder.cpp)
add_test(NAME encoder COMMAND encoder)
target_link_libraries(encoder PRIVATE mediafx Qt::Test)

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

add_qml_test(qml_video_clipstart 15:320x180 video-clipstart.qml video-clipstart.nut nut:red:320x180:15:8)
add_qml_test(qml_multisink 30:640x360 multisink.qml multisink.nut nut:red:640x360:30:4 png:red:160x120)

# Label tests that require a GPU
set_tests_properties(qml_video_clipstart qml_multisink PROPERTIES LABELS GPU)