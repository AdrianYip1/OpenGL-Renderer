#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;

// use the a component of gAlbedoSpec as 0/1 for specular map
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
};
uniform Material material;

void main() {
    gPosition = vec4(FragPos, 1.0);

    vec3 normalSample = texture(material.texture_normal1, TexCoords).rgb;
    gNormal = normalize(TBN * normalize(normalSample * 2.0 - 1.0));
    
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = texture(material.texture_specular1, TexCoords).r;
}