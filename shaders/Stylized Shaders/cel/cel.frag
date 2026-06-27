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

uniform vec3 lightTint;
uniform vec3 shadowTint;
uniform vec3 ambientTint;

uniform float uSaturation;


void main() {

    // Early exit check for background
    vec4 posData = texture(geometryPass.gPosTexture, TexCoords);
    if (posData.a == 0.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 fragPos = posData.rgb;
    vec3 viewDir = normalize(uCameraPos - fragPos);
    vec4 texColor = texture(geometryPass.gAlbedoSpecTexture, TexCoords);
    texColor.rgb *= ambientTint;

// Diffuse with hard cutoff
    vec3 normal = texture(geometryPass.gNormalTexture, TexCoords).rgb;
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(normal, lightDir);
    float lightIntensityDir = step(0.3, NDiff);

// Distinct colour for lit and shadow areas
    vec4 shadowTinted = vec4(clamp(texColor.rgb * shadowTint, 0.0, 1.0), 1.0);
    vec4 targetColor = mix(shadowTinted, texColor, lightIntensityDir) * vec4(lightTint, 1.0);
    targetColor.rgb *= mix(vec3(1.0), shadowTint, 1.0 - lightIntensityDir);

// Specular (Use half vector -> Blinn-Phong model)
    float specMap = texColor.a;
    vec3 halfVect = normalize(viewDir + lightDir);
    float NSpec = max(dot(normal, halfVect), 0.0) * lightIntensityDir;
    float spec = pow(NSpec, uGlossiness * uGlossiness);
    float specSmooth = smoothstep(0.3, 0.32, spec);
    vec4 specular = uSpecular * specSmooth * specMap;

// Saturation
    float LUM = dot(targetColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    targetColor.rgb = mix(vec3(LUM), targetColor.rgb, uSaturation); //grey when uSat = 0, norma when uSat = 1

// Rim Lighting
    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = step(0.75, rim);
    rim *= clamp(-NDiff + 0.5, 0.0, 1.0); // stronger on shadow-facing edges
    vec4 rimColor = rim * uRimColor * (bRimColor ? 1.0 : 0.0);


    FragColor = targetColor + rimColor + specular;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}