#version 330 core

uniform float iTime;
uniform vec2 iResolution;

out vec4 FragColor;

vec3 palette(float t) {
    vec3 a = vec3(0.408, 0.500, 0.500);
    vec3 b = vec3(-0.252, 0.500, 0.338);
    vec3 c = vec3(-0.672, 0.500, 0.333);
    vec3 d = vec3(-0.022, 0.500, 0.667);
    return a + b * cos(6.28318 * (c * t + d));
}


void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution.xy) / iResolution.y;
  
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);
    
    for (float i = 0.0; i < 4.; i++) {
    
        uv = fract(uv * 1.61803398875) - 0.5;
        
        vec3 col = palette(length(uv0) + iTime * .4);
        float d = length(uv) * exp(-length(uv0));
        
        d = sin(d *10.0 + iTime) / 10.0;
        d = abs(d);
        d = pow(0.01/ d, 2.0);

        finalColor += d * col;
    }
    
    FragColor = vec4(finalColor, 1.0);
}
