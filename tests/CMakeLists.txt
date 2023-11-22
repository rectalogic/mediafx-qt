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

target_link_libraries(interval PRIVATE libmediafx Qt::Test)