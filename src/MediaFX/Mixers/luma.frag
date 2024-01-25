// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float softness;
    float softTime;
    int invert; // bool not supported by legacy shaders
};
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D dest;
layout(binding = 3) uniform sampler2D luma;
void main() {
    vec4 sourceColor = texture(source, qt_TexCoord0);
    vec4 destColor = texture(dest, qt_TexCoord0);
    float lumaF = dot(texture(luma, qt_TexCoord0).rgb, vec3(0.2126, 0.7152, 0.0722));
    if (invert > 0)
        lumaF = 1.0 - lumaF;
    // Based on https://github.com/j-b-m/movit/blob/master/luma_mix_effect.frag
    float m = clamp((lumaF * softness - softness) + softTime, 0.0, 1.0);
    fragColor = mix(sourceColor, destColor, m) * qt_Opacity;
}