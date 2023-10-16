# Copyright (c) 2023 Andrew Wason, All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia Qml Quick Quick3D)

qt_policy(SET QTP0001 NEW)

qt_add_executable(mediafx
    main.cpp
    session.cpp
    mediafx.cpp
    render_control.cpp
    clip.cpp
    media_clip.cpp
    image_clip.cpp
)

qt_add_qml_module(mediafx
    URI stream.mediafx
)

target_link_libraries(mediafx PRIVATE Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::Multimedia Qt6::Qml Qt6::Quick Qt6::Quick3D)

install(TARGETS mediafx RUNTIME DESTINATION)
