#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

uniform mat4x4 projMatrix;
uniform mat4x4 viewMatrix;
uniform mat4x4 modelMatrix;

uniform sampler2DArray textureList;
uniform int textureIdx;

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
