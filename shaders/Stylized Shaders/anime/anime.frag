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

uniform vec4 uSpecular;
uniform float uGlossiness;
uniform vec4 uRimColor;

void main() {

    vec4 texColor = texture(material.texture_diffuse1, TexCoord);

// Diffuse with hard cutoff
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(norm, lightDir);
    float lightIntensityDir = step(0.3, NDiff);

// Distinct colour for lit and shadow areas
    vec4 shadowTinted = vec4(clamp(texColor.rgb * 0.85, 0.0, 1.0), 1.0);
    vec4 targetColor = mix(shadowTinted, texColor, lightIntensityDir);

// Specular (Use half vector -> Blinn-Phong model)
    vec3 viewDir = normalize(ViewDir);
    vec3 halfVect = normalize(viewDir + lightDir);
    float NSpec = max(dot(norm, halfVect), 0.0) * lightIntensityDir;
    float spec = pow(NSpec, uGlossiness * uGlossiness);
    float specSmooth = smoothstep(0.3, 0.32, spec);
    vec4 specular = uSpecular * specSmooth;

// Rim Lighting
    float rim = 1 - max(dot(viewDir, norm), 0.0);
    rim = step(0.5, rim);
    vec4 rimColor = rim * uRimColor * 0.5;


    FragColor = targetColor + rimColor + specular;
}