// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Screen-space quad vertex pass-through
// - Multi-sample radial/directional motion blur 
// - Blur intensity dynamically mapped to vehicle velocity
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
out vec4 fColor;

uniform sampler2D sampler0;
uniform float blurAmount; // Driven by car speed

void main() {
    vec2 center = vec2(0.5, 0.5);
    vec2 dir = vTexCoord - center;
    vec4 color = vec4(0.0);
    const int samples = 10;
    for(int i = 0; i < samples; i++) {
        color += texture(sampler0, vTexCoord + dir * float(i) * blurAmount);
    }
    fColor = color / float(samples);
}