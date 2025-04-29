#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;

uniform mat4x4 projMatrix;
uniform mat4x4 viewMatrix;
uniform mat4x4 modelMatrix;

out vec3 fragPos;
out vec3 fragNormal;

void main() {
    vec4 modelPos = modelMatrix * vec4(vertPos, 1.0);
    vec4 modelNormal = modelMatrix * vec4(vertNormal, 1.0);

    vec4 worldPos = projMatrix * viewMatrix * modelPos;
    // vec4 worldNormal = projMatrix * viewMatrix * modelNormal;

    fragNormal = modelNormal.xyz;
    fragPos = modelPos.xyz;

    gl_Position = worldPos;
}
