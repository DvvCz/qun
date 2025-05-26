#version 450 core

in vec2 fragPos;
in vec2 fragUV;

layout(location = 0) uniform sampler2DArray textureList;
layout(location = 1) uniform int textureIdx;

layout(std140, binding = 0) uniform MaterialBlock {
    vec3 materialColor;
};

out vec4 outColor;

void main() {
    outColor = vec4(materialColor, 1.0);
}
