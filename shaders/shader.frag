#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform float time;

void main() {
    // starter: visualize normals as colour
    vec3 color = Normal * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}
