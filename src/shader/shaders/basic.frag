#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

out vec4 outColor;

void main() {
    vec3 positionColor = fragPos * 0.5 + 0.5;
    vec3 normalColor = fragNormal * 0.5 + 0.5;

    outColor = vec4(positionColor * 0.5 + normalColor * 0.5, 1.0);
}
