#version 450 core

struct Texture {
    vec2 uvScale;
    vec2 uvOffset;

    /// -1 if no texture
    int index;

    /// Rotation in radians
    float uvRotation;
};

in vec2 fragPos;
in vec2 fragUV;

layout(location = 0) uniform sampler2DArray textureList;
layout(location = 1) uniform int textureIdx;

layout(std140, binding = 0) uniform MaterialBlock {
    vec3 materialColor;
    Texture materialTexture;
};

out vec4 outColor;

vec2 transformUV(vec2 uv, Texture tex) {
    vec2 transformedUV = uv;
    transformedUV *= tex.uvScale;
    transformedUV += tex.uvOffset;

    return transformedUV;
}

void main() {
    if (materialTexture.index >= 0) {
        // Transform UV coordinates
        vec2 uv = transformUV(fragUV, materialTexture);

        // Sample the texture
        vec4 texColor = texture(textureList, vec3(uv, materialTexture.index));

        // Apply alpha test
        if (texColor.a < 0.5) {
            discard; // Discard fragments with low alpha
        }

        // Combine texture color with material color
        outColor = vec4(materialColor * texColor.rgb, texColor.a);
    } else {
        outColor = vec4(materialColor, 1.0);
    }
}
