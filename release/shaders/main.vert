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
    vec4 modelNormal = modelMatrix * vec4(vertNormal, 1.0);

    vec4 worldPos = projMatrix * viewMatrix * modelPos;
    vec4 worldNormal = projMatrix * viewMatrix * modelNormal;

    fragNormal = modelNormal.xyz;
    fragPos = modelPos.xyz;
    fragUV = vertUV;

    gl_Position = worldPos;
}
