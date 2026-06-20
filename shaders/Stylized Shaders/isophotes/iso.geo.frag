#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

struct Material {
    sampler2D texture_normal1;
};
uniform Material material;

void main() {
    gPosition = vec4(FragPos, 1.0);

    vec3 normalSample = texture(material.texture_normal1, TexCoords).rgb;
    gNormal = vec4(normalize(TBN * normalize(normalSample * 2.0 - 1.0)), 0.0);
}