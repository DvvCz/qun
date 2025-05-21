#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 outColor;

void main() {
    outColor = vec4(vec3(0.5, 0.5, 0.5), 1.0); // Set color to gray
    // outColor = vec4(fragNormal * 0.5 + 0.5, 1.0);
}
