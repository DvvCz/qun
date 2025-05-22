#version 330 core

struct Light {
    vec3 position;
    vec3 color;
}

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform int textureIdx;

#define MAX_LIGHTS 20

layout(std140, location = 0) uniform LightBlock {
    uint lightCount;
    Light lights[MAX_LIGHTS];
}

out vec4 outColor;

void main() {
    // Sample from texture array using fragUV and the texture index
    outColor = texture(textureList, vec3(fragUV, float(textureIdx)));
}

// void main() {
//     vec3 positionColor = fragPos * 0.5 + 0.5;
//     vec3 normalColor = fragNormal * 0.5 + 0.5;

//     outColor = vec4(positionColor * 0.5 + normalColor * 0.5, 1.0);
// }
