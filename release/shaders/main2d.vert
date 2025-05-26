#version 450 core

in vec2 vertPos;
in vec2 vertUV;

layout(location = 0) uniform sampler2DArray textureList;
layout(location = 1) uniform int textureIdx;

out vec2 fragPos;
out vec2 fragUV;

void main() {
    fragPos = vertPos;
    fragUV = vertUV;

    gl_Position = vec4(fragPos, 0.0, 1.0);
}
