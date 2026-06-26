#version 330 core

in vec2 TexCoords;

struct GeometryPass {
    sampler2D gPosTexture;
    sampler2D gNormalTexture;
};
uniform GeometryPass geometryPass;

uniform vec3 uCameraPos;
uniform vec3 uDirectionalLight;

uniform float uLineWidth;

out vec4 FragColor;

void main() {
    
    // dot (N, L) then threshold it
    //on the threshold, draw black line

    vec3 normal = texture(geometryPass.gNormalTexture, TexCoords).rgb;
    vec3 lightDir = normalize(-uDirectionalLight);
    float NDiff = dot(normal, lightDir);

    float eps = fwidth(NDiff) * uLineWidth;

    float threshold[4] = float[](0.2, 0.4, 0.6, 0.8);

    FragColor = vec4(1.0, 1.0, 1.0, 1.0) - vec4(0.1, 0.1, 0.1, 1.0);

    for (int i = 0; i < 4; i++) {
        if (abs(NDiff - threshold[i]) < eps) {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            break;
        }
    }
}