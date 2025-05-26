#version 450 core

in vec2 fragPos;
in vec2 fragUV;

layout(location = 0) uniform sampler2DArray textureList;
layout(location = 1) uniform int textureIdx;

out vec4 outColor;

void main() {
    outColor = vec4(vec3(1.0, 1.0, 1.0), 1.0);
}
