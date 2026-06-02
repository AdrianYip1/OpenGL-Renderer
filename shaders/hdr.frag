#version 330 core

uniform float exposure;
uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;

in vec2 TexCoords;
out vec4 FragColor;

void main() {

    const float gamma = 2.2;
    vec3 hdr = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloom = texture(bloomBlur, TexCoords).rgb;
    hdr += bloom;
    vec3 mapped = vec3(1.0) - exp(-hdr * exposure); // change to aces tonemapping

    mapped = pow(mapped, vec3(1.0/gamma));
     
    FragColor = vec4(mapped, 1.0);
}