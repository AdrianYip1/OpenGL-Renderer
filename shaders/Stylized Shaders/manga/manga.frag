#version 330 core

in vec2 TexCoords;

struct GeometryPass {
    sampler2D gPosTexture;
    sampler2D gNormalTexture;
};
uniform GeometryPass geometryPass;

uniform vec3 uCameraPos;
uniform vec3 uDirectionalLight;

out vec4 FragColor;

float diagLine1() {
    return (mod(gl_FragCoord.x - gl_FragCoord.y, 10.0) < 1.0) ? 0.0 : 1.0;
}

float diagLine2() {
    return (mod(gl_FragCoord.x + gl_FragCoord.y, 10.0) < 1.0) ? 0.0 : 1.0;
}

float horiLine() {
    return (mod(gl_FragCoord.y, 10.0) < 1.0) ? 0.0 : 1.0;
}

void main() {

    // Background checks pixels with no mesh behind them for early exit
    vec4 posData = texture(geometryPass.gPosTexture, TexCoords);
    if (posData.a == 0.0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    vec3 fragPos = posData.rgb;
    vec3 viewDir = normalize(uCameraPos - fragPos);

    vec3 normal = texture(geometryPass.gNormalTexture, TexCoords).rgb;
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(normal, lightDir);

    float colourValue;

    if (NDiff < 0.3) {
        float d1 = diagLine1();
        float d2 = diagLine2();
        float hl = horiLine();
        colourValue = min(min(d1, d2), hl);
    }
    else if (NDiff < 0.5) {
        float d1 = diagLine1();
        float hl = horiLine();
        colourValue = min(d1, hl);
    }
    else if (NDiff < 0.7) {
        colourValue = diagLine2();
    }
    else {
        colourValue = 1.0;
    }

    FragColor = vec4(colourValue, colourValue, colourValue, 1.0);
}