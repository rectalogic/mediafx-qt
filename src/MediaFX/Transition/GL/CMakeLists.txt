# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(gltransition STATIC)

function(process_gltransition_shader)
    cmake_parse_arguments(GLTRANSITION "" "" "SHADERS" ${ARGN})

    foreach(GLTRANSITION_SHADER ${GLTRANSITION_SHADERS})
        file(READ ${GLTRANSITION_SHADER} SHADER_TRANSITION)
        string(REGEX REPLACE "(uniform [^\\;]+)\\;" "\\1\\\;" SHADER_TRANSITION "${SHADER_TRANSITION}")
        string(REGEX MATCHALL "uniform [^\n]+\n" SHADER_UNIFORMS "${SHADER_TRANSITION}")

        # QML does not support integer uniforms
        # https://bugreports.qt.io/browse/QTBUG-124805
        string(REGEX REPLACE "uniform ivec2" "uniform vec2" SHADER_UNIFORMS "${SHADER_UNIFORMS}")
        string(REGEX REPLACE "uniform int" "uniform float" SHADER_UNIFORMS "${SHADER_UNIFORMS}")

        list(JOIN SHADER_UNIFORMS "\n" SHADER_UNIFORMS)
        string(REGEX REPLACE "uniform [^\n]+\n" "" SHADER_TRANSITION "${SHADER_TRANSITION}")
        cmake_path(GET GLTRANSITION_SHADER FILENAME GLTRANSITION_SHADER_FILENAME)
        configure_file(gltransition.frag.in ${GLTRANSITION_SHADER_FILENAME}.frag @ONLY)
    endforeach()

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${GLTRANSITION_SHADERS})
endfunction()

set(SHADERNAMES
    doorway.glsl
    ripple.glsl
    windowslice.glsl
    SimpleZoom.glsl
    LinearBlur.glsl
    WaterDrop.glsl
    GlitchMemories.glsl
    PolkaDotsCurtain.glsl
    StereoViewer.glsl
    luminance_melt.glsl
    perlin.glsl
    directionalwarp.glsl
    Bounce.glsl
    morph.glsl
    ColourDistance.glsl
    Swirl.glsl
    CrossZoom.glsl
    Dreamy.glsl
    GridFlip.glsl
    ZoomInCircles.glsl
    Mosaic.glsl
    undulatingBurnOut.glsl
    crosshatch.glsl
    CrazyParametricFun.glsl
    ButterflyWaveScrawler.glsl
    kaleidoscope.glsl
    windowblinds.glsl
    hexagonalize.glsl
    GlitchDisplace.glsl
    DreamyZoom.glsl
    DoomScreenTransition.glsl
    burn.glsl
    colorphase.glsl
    crosswarp.glsl
    cube.glsl
    fadecolor.glsl
    fadegrayscale.glsl
    flyeye.glsl
    multiply_blend.glsl
    pixelize.glsl
    randomsquares.glsl
    rotate_scale_fade.glsl
    squareswire.glsl
    squeeze.glsl
    swap.glsl
    wind.glsl
)

list(TRANSFORM SHADERNAMES PREPEND gl-transitions/transitions/ OUTPUT_VARIABLE SHADERPATHS)
process_gltransition_shader(SHADERS ${SHADERPATHS})

list(TRANSFORM SHADERNAMES PREPEND ${CMAKE_CURRENT_BINARY_DIR}/ OUTPUT_VARIABLE FRAGPATHS)
list(TRANSFORM FRAGPATHS APPEND .frag)
list(TRANSFORM SHADERNAMES REPLACE "glsl$" "" OUTPUT_VARIABLE QSBPATHS)
list(TRANSFORM QSBPATHS APPEND frag.qsb)

# https://bugreports.qt.io/browse/QTBUG-103723
qt_add_shaders(gltransition "gltransitionshaders"
    GLSL "300es,330"
    PREFIX
    "/shaders/gltransition"
    FILES
    ${FRAGPATHS}
    gltransition.vert
    OUTPUTS
    ${QSBPATHS}
    gltransition.vert.qsb
    OUTPUT_TARGETS gltransitionshader_output_targets
)

foreach(shader_output_target ${gltransitionshader_output_targets})
    add_dependencies(gltransition ${shader_output_target})
endforeach()

qt_add_qml_module(gltransition
    URI MediaFX.Transition.GL
    QML_FILES
    TransitionShaderEffect.qml
    Doorway.qml
    Ripple.qml
    WindowSlice.qml
    SimpleZoom.qml
    LinearBlur.qml
    WaterDrop.qml
    GlitchMemories.qml
    PolkaDotsCurtain.qml
    StereoViewer.qml
    LuminanceMelt.qml
    Perlin.qml
    DirectionalWarp.qml
    Bounce.qml
    Morph.qml
    ColourDistance.qml
    Swirl.qml
    CrossZoom.qml
    Dreamy.qml
    GridFlip.qml
    ZoomInCircles.qml
    Mosaic.qml
    UndulatingBurnOut.qml
    CrossHatch.qml
    CrazyParametricFun.qml
    ButterflyWaveScrawler.qml
    Kaleidoscope.qml
    WindowBlinds.qml
    Hexagonalize.qml
    GlitchDisplace.qml
    DreamyZoom.qml
    DoomScreen.qml
    Burn.qml
    ColorPhase.qml
    CrossWarp.qml
    Cube.qml
    FadeColor.qml
    FadeGrayscale.qml
    FlyEye.qml
    MultiplyBlend.qml
    Pixelize.qml
    RandomSquares.qml
    RotateScaleFade.qml
    SquaresWire.qml
    Squeeze.qml
    Swap.qml
    Wind.qml
)