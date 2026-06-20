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

float detectEdge(vec2 uv) {
    vec2 texel = 1.0 / vec2(textureSize(geometryPass.gNormalTexture, 0));

    vec3 n = texture(geometryPass.gNormalTexture, uv).rgb;
    vec3 p = texture(geometryPass.gPosTexture, uv).rgb;

    vec2 off[4] = vec2[](vec2(texel.x, 0), vec2(-texel.x, 0), vec2(0, texel.y), vec2(0, -texel.y));

    float normalEdge = 0.0;
    float depthEdge = 0.0;

    for (int i =0; i < 4; i++) {
        vec3 nN = texture(geometryPass.gNormalTexture, uv + off[i]).rgb;
        vec3 pN = texture(geometryPass.gPosTexture, uv + off[i]).rgb;
        normalEdge += 1.0 - max(dot(n, nN), 0.0);
        depthEdge += length(p - pN);
    }

    float e = step(0.3, normalEdge) + step(0.2, depthEdge);
    return clamp(e, 0.0, 1.0);
}

float screentone(float density) {
    float scale = 6.0;
    vec2 p = gl_FragCoord.xy / scale;
    // rotate so dots aren't axis-aligned
    float a = 0.5; mat2 R = mat2(cos(a),-sin(a),sin(a),cos(a));
    p = R * p;
    float d = length(fract(p) - 0.5);
    return step(density * 0.7, d); // smaller density -> bigger black dots
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
        colourValue = screentone(0.2);
    }
    else {
        colourValue = 1.0;
    }

    colourValue *= (1.0 - detectEdge(TexCoords));
    FragColor = vec4(colourValue, colourValue, colourValue, 1.0);
}