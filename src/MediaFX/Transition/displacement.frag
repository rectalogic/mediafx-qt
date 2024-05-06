// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later
#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
    float strength;
};
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D dest;
layout(binding = 3) uniform sampler2D displacementMap;

void main() {
    float displacement = dot(texture(displacementMap, qt_TexCoord0).rgb, vec3(0.2126, 0.7152, 0.0722)) * strength;

    vec2 uvFrom = vec2(qt_TexCoord0.x + time * displacement, qt_TexCoord0.y);
    vec2 uvTo = vec2(qt_TexCoord0.x - (1.0 - time) * displacement, qt_TexCoord0.y);

    fragColor = mix(texture(source, uvFrom), texture(dest, uvTo), time) * qt_Opacity;
}