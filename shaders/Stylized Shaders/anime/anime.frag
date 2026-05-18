#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 ViewDir;
in vec3 Normal;

struct Material {
    sampler2D texture_diffuse1;
};
uniform Material material;

uniform vec3 uDirectionalLight;
uniform vec4 uShadowColour;
uniform vec4 uSpecular;
uniform float uGlossiness;

void main() {

    vec4 texColor = texture(material.texture_diffuse1, TexCoord);

// Diffuse with hard cutoff
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(norm, lightDir);
    float lightIntensityDir = step(0.1, NDiff);

// Distinct colour for lit and shadow areas
    vec4 targetColor = mix(uShadowColour, texColor, lightIntensityDir);

// Specular (Use half vector -> Blinn-Phong model)
    vec3 viewDir = normalize(ViewDir);
    vec3 halfVect = normalize(viewDir + lightDir);
    float NSpec = max(dot(norm, halfVect), 0.0);
    float spec = pow(NSpec, uGlossiness);
    spec = step(0.7, spec);
    vec4 specular = uSpecular * spec;


// Rim Lighting

//Outlines
    FragColor = targetColor + specular;
}