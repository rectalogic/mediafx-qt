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

find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia Qml Quick Quick3D)

qt_policy(SET QTP0001 NEW)

qt_add_executable(mediafx
    main.cpp
    session.cpp
    mediafx.cpp
    render_control.cpp
    clip.cpp
    visual_clip.cpp
    video_clip.cpp
    image_clip.cpp
)

qt_add_qml_module(mediafx
    URI stream.mediafx
)

target_link_libraries(mediafx PRIVATE Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::Multimedia Qt6::Qml Qt6::Quick Qt6::Quick3D)

install(TARGETS mediafx RUNTIME DESTINATION)
