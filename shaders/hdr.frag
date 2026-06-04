#version 330 core

uniform float exposure;
uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform int HDRtype;

in vec2 TexCoords;
out vec4 FragColor;

vec3 exposureTonemap(vec3 x) {
    return vec3(1.0) - exp(-x * exposure);
}

vec3 acesTonemap(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Define constant values for GT Tonemapping
const float P = 1.0; // max output brightness
const float m = 0.22; // where toe ends and linear begins
const float l = 0.40; // length of the linear section
const float a = 1.0; // slope of the linear section
const float c = 1.33; // toe curve strength
const float b = 0.0; // black offset

float gtTonemap(float x) {
    float l0 = (P - m) * l / a; // derived width of linear section
    float S0 = m + l0; // brightness where shoulder begins
    float S1 = m + a * l0; // trajectory point for shoulder curve
    float C2 = a * P / (P - S1); // compression rate of shoulder

    float T_x = m * pow(x / m, c) + b; // toe
    float L_x = m + a * (x - m); // linear
    float S_x = P - (P - S1) * exp(-C2 * (x - S0) / P); // shoulder

    float w0 = 1.0 - smoothstep(0.0, m, x);  // toe weight
    float w2 = (x < m + l) ? 0.0 : 1.0; // shoulder weight
    float w1 = 1.0 - w0 - w2; // linear weight

    return T_x * w0 + L_x * w1 + S_x * w2;
}

void main() {
    const float gamma = 2.2;
    vec3 hdr = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloom = texture(bloomBlur, TexCoords).rgb;
    hdr += bloom;

    vec3 mapped;

    // HDR options
    if (HDRtype == 0) mapped = exposureTonemap(hdr);
    else if (HDRtype == 1) mapped = acesTonemap(hdr * exposure);
    else mapped = vec3(gtTonemap(hdr.r * exposure), gtTonemap(hdr.g * exposure), gtTonemap(hdr.b * exposure));

    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}
