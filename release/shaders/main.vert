#version 450 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform vec3 cameraPos;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;

void main() {
    vec4 modelPos = modelMatrix * vec4(vertPos, 1.0);

    // Normal matrix is the inverse transpose of the upper-left 3x3 of the model matrix
    // For uniform scaling or when modelMatrix is orthogonal, we can use mat3(modelMatrix)
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 transformedNormal = normalize(normalMatrix * vertNormal);

    vec4 worldPos = projMatrix * viewMatrix * modelPos;

    fragNormal = transformedNormal;
    fragPos = modelPos.xyz;
    fragUV = vertUV;

    gl_Position = worldPos;
}
