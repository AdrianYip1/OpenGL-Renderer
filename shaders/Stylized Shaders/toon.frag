#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 viewPos;


uniform float time;
uniform vec3 dirLight;
uniform vec4 uColor;
uniform vec4 uAmbient;
uniform vec4 uDirectional;
uniform vec4 uSpecular;
uniform vec4 uRimColor;
uniform float uGlossiness;

void main() {

    vec3 dirLightNorm = normalize(-dirLight); //flip direction
    vec3 viewDir = normalize(viewPos - FragPos);

    float NDotL = dot(dirLightNorm, Normal);
    float lightIntensityCartoon = smoothstep(0.0, 0.01, NDotL);

    vec3 halfVector = normalize(viewDir + dirLightNorm);

    float NDotH = dot(Normal, halfVector);
    float specularIntensity = pow(NDotH * lightIntensityCartoon, uGlossiness * uGlossiness);
    float specularIntensitySmooth = smoothstep(0.0, 0.02, specularIntensity);
    vec4 specular = uSpecular * specularIntensitySmooth;

    float rimDot = 1.0 - dot(Normal, viewDir);
    float rimIntensity = rimDot * pow(max(NDotL, 0.0), 0.1);
    vec4 rim = uRimColor * rimIntensity;

    vec4 light = lightIntensityCartoon * uDirectional;

    FragColor = uColor * (light + uAmbient + specular + rim);
}
