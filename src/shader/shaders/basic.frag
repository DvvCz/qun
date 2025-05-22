#version 450 core

struct Light {
    vec3 position;
    vec3 color;
};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform int textureIdx;

#define MAX_LIGHTS 20

layout(std140, binding = 0) uniform LightBlock {
    uint lightCount;
    Light lights[MAX_LIGHTS];
};

out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);

    vec3 ambientColor = texture(textureList, vec3(fragUV, float(textureIdx))).rgb;
    vec3 ambient = 0.2 * ambientColor;

    vec3 diffuse = vec3(0.0);
    for (uint i = 0; i < lightCount; i++) {
        vec3 lightDir = normalize(lights[i].position - fragPos);
        float intensity = max(dot(normal, lightDir), 0.0);

        diffuse += intensity * lights[i].color;
    }

    vec3 resultColor = ambient + diffuse;

    outColor = vec4(resultColor, 1.0);
}
