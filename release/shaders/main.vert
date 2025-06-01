#version 450 core

struct Texture {
    int index;

    vec2 uvScale;
    vec2 uvOffset;

    /// Rotation in radians
    float uvRotation;
};

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;
layout(location = 3) in vec3 vertTangent;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform vec3 cameraPos;

layout(std140, binding = 1) uniform Material {
    vec3 materialAmbient;
    vec3 materialDiffuse;
    vec3 materialSpecular;
    float materialShininess;
    float materialDissolve;
    Texture diffuseTexture;
    Texture normalTexture;
};

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;
out mat3 fragTBN;

void main() {
    vec4 modelPos = modelMatrix * vec4(vertPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 transformedNormal = normalize(normalMatrix * vertNormal);

    // Only calculate TBN if normal map present
    if (normalTexture.index >= 0) {
        vec3 transformedTangent = normalize(normalMatrix * vertTangent);
        vec3 transformedBitangent = normalize(cross(transformedNormal, transformedTangent));

        vec3 T = normalize(transformedTangent);
        vec3 B = normalize(transformedBitangent);
        vec3 N = normalize(transformedNormal);
        fragTBN = mat3(T, B, N);
    }

    vec4 worldPos = projMatrix * viewMatrix * modelPos;

    fragNormal = transformedNormal;
    fragPos = modelPos.xyz;
    fragUV = vertUV;

    gl_Position = worldPos;
}
