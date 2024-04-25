// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

#version 440

// Based on the default https://github.com/qt/qtdeclarative/blob/dev/src/quick/scenegraph/shaders_ng/shadereffect.vert

layout(location = 0) in vec4 qt_Vertex;
layout(location = 1) in vec2 qt_MultiTexCoord0;

layout(location = 0) out vec2 qt_TexCoord0;

layout(std140, binding = 0) uniform qt_buf {
    mat4 qt_Matrix;
    float qt_Opacity;
} qt_ubuf; // must use a name that does not clash with custom code when no uniform blocks

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    qt_TexCoord0 = qt_MultiTexCoord0;
    // Mirror everything vertically, requires textures to not mirror layer.textureMirroring: ShaderEffectSource.NoMirroring
    qt_TexCoord0.y = (qt_TexCoord0.y * -1.0) + 1.0;
    gl_Position = qt_ubuf.qt_Matrix * qt_Vertex;
}
