#version 330 core

in vec3 FragPos;
out vec4 FragColor;
uniform vec2 uResolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / uResolution.y;
    uv = (uv * 2.0) - 1.0;     
    
    // CrossHatch
    bool vertStripes = fract(uv.x * 10.0) > 0.05;
    bool horiStripes = fract(uv.y * 10.0) > 0.05;
    bool diagStripes = fract((uv.x - uv.y) * 10.0) > 0.05;
    float stripes = (!vertStripes || !horiStripes || !diagStripes) ? 0.0 : 1.0;
    // Output to screen
    FragColor = vec4(stripes, stripes, stripes, 1.0);
}