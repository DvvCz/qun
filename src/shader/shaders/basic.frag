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

layout(location = 5) uniform float cameraPos;

#define MAX_LIGHTS 20

layout(std140, binding = 0) uniform LightBlock {
    uint lightCount;
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform MaterialBlock {
    vec3 materialAmbient;
    vec3 materialDiffuse;
    vec3 materialSpecular;
    float materialShininess;
    float materialDissolve;
};

out vec4 outColor;

void main() {
    vec3 fragToCameraDir = normalize(cameraPos - fragPos);
    vec3 normal = normalize(fragNormal);

    vec3 ambientColor = texture(textureList, vec3(fragUV, float(textureIdx))).rgb;
    vec3 ambient = ambientColor;

    vec3 diffuse = vec3(0.0);
    for (uint i = 0; i < lightCount; i++) {
        vec3 lightDir = normalize(lights[i].position - fragPos);
        float intensity = max(dot(normal, lightDir), 0.0);

        diffuse += intensity * lights[i].color;
    }

    vec3 specular = vec3(0.0);
    for (uint i = 0; i < lightCount; i++) {
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(fragToCameraDir, reflectDir), 0.0), materialShininess);

        specular += spec * lights[i].color;
    }

    vec3 resultColor = 0.5 * materialAmbient * ambient + materialDiffuse * diffuse + materialSpecular * specular;
    outColor = vec4(resultColor, materialDissolve);
}
