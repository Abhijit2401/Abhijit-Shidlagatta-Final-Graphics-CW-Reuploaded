// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Procedural wind sway animation based on global time (t)
// - Alpha-discard rendering for transparent grass billboards
// - Color tinting and distance fog integration
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
in vec4 vEyePos;
in vec3 vEyeNorm;

out vec4 vOutputColour;

uniform sampler2D sampler0;
uniform vec3 sunColor;

void main() {
    vec4 baseTex = texture(sampler0, vTexCoord);
    if(baseTex.a < 0.1) discard;
    vec4 greenTint = vec4(0.4, 0.7, 0.3, 1.0);
    vec4 finalTex = baseTex * greenTint;
    vec3 ambientLight = sunColor * 0.8; 
    vOutputColour = finalTex * vec4(ambientLight, 1.0f);
    float dist = length(vEyePos.xyz);
    float fogFactor = clamp((1500.0 - dist) / (1000.0 - 200.0), 0.0, 1.0);
    vOutputColour = mix(vec4(0.85, 0.55, 0.65, 0.85), vOutputColour, fogFactor);
}