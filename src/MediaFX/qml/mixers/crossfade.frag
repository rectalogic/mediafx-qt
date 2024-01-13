// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
};
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D dest;
void main() {
    vec4 sp = texture(source, qt_TexCoord0);
    vec4 dp = texture(dest, qt_TexCoord0);
    fragColor = mix(sp, dp, time) * qt_Opacity;
}