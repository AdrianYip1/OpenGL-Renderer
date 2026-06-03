#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

struct GeometryPass {
    sampler2D gPosTexture;
    sampler2D gNormalTexture;
    sampler2D gAlbedoSpecTexture;
};
uniform GeometryPass geometryPass;

uniform vec3 uCameraPos;
uniform vec3 uDirectionalLight;

uniform vec4 uSpecular;
uniform float uGlossiness;

uniform bool bRimColor;
uniform vec4 uRimColor;

void main() {
    vec3 fragPos = texture(geometryPass.gPosTexture, TexCoords).rgb;
    vec3 viewDir = normalize(uCameraPos - fragPos);
    vec4 texColor = texture(geometryPass.gAlbedoSpecTexture, TexCoords);

// Diffuse with hard cutoff
    vec3 normal = texture(geometryPass.gNormalTexture, TexCoords).rgb;
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(normal, lightDir);
    float lightIntensityDir = step(0.3, NDiff);

// Distinct colour for lit and shadow areas
    vec4 shadowTinted = vec4(clamp(texColor.rgb * 0.85, 0.0, 1.0), 1.0);
    vec4 targetColor = mix(shadowTinted, texColor, lightIntensityDir);

// Specular (Use half vector -> Blinn-Phong model)
    float specMap = texColor.a;
    vec3 halfVect = normalize(viewDir + lightDir);
    float NSpec = max(dot(normal, halfVect), 0.0) * lightIntensityDir;
    float spec = pow(NSpec, uGlossiness * uGlossiness);
    float specSmooth = smoothstep(0.3, 0.32, spec);
    vec4 specular = uSpecular * specSmooth * specMap;

// Rim Lighting
    float rim = 1 - max(dot(viewDir, normal), 0.0);
    rim = step(0.5, rim);
    vec4 rimColor = rim * uRimColor * 0.5 * (bRimColor ? 1.0 : 0.0);


    FragColor = targetColor + rimColor + specular;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 2.5) {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}