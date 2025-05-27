#version 450 core

struct Light {
    vec3 position;
    vec3 color;
    float radius;
};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform vec3 cameraPos;

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
    int diffuseTextureIdx;
};

out vec4 outColor;

void main() {
    vec3 fragToCameraDir = normalize(cameraPos - fragPos);
    vec3 normal = normalize(fragNormal);

    vec3 ambient = vec3(1.0);
    if (diffuseTextureIdx >= 0) {
        ambient = texture(textureList, vec3(fragUV, float(diffuseTextureIdx))).rgb;
    }

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (uint i = 0; i < lightCount; i++) {
        // Calculate light direction and distance
        vec3 lightToFrag = fragPos - lights[i].position;
        vec3 lightDir = -normalize(lightToFrag);

        // Diffuse calculation
        float diff = max(dot(normal, lightDir), 0.0);

        // Specular calculation - only calculate if light is hitting the front face
        float spec = 0.0;
        if (diff > 0.0) {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(fragToCameraDir, reflectDir), 0.0), materialShininess);
        }

        // Avoid extreme light at very close distances
        float distToLight = length(lightToFrag);
        float distAttenuation = 1.0 / (1.0 + 0.09 * distToLight + 0.032 * distToLight * distToLight);

        diffuse += diff * lights[i].color * distAttenuation;
        specular += spec * lights[i].color * distAttenuation;
    }

    // Balance the different lighting components
    vec3 ambientPart = materialAmbient * ambient;
    vec3 diffusePart = materialDiffuse * diffuse;
    vec3 specularPart = materialSpecular * specular;

    vec3 resultColor = ambientPart + diffusePart + specularPart;
    outColor = vec4(resultColor, materialDissolve);
}
