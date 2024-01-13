// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float transitionWidth;
    float premultipliedTransitionWidth;
};
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D dest;
layout(binding = 3) uniform sampler2D luma;
void main() {
    vec4 sp = texture(source, qt_TexCoord0);
    vec4 dp = texture(dest, qt_TexCoord0);
    vec4 lp = texture(luma, qt_TexCoord0);
    
    // Based on https://github.com/j-b-m/movit/blob/master/luma_mix_effect.frag
    // premultipliedTransitionWidth = time * (transitionWidth + 1.0)
    float m = clamp((lp.r * transitionWidth - transitionWidth) + premultipliedTransitionWidth, 0.0, 1.0);
    fragColor = mix(sp, dp, m) * qt_Opacity;
}