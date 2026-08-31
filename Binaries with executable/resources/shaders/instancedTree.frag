// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Hardware Instancing coordinate offsets via gl_InstanceID
// - Mathematical exclusion zones (preventing trees spawning on the track)
// - Procedural stretching and scaling per-instance
// - Custom fake lighting and distance fog blending
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
in vec4 vEyePos;
in vec3 vEyeNorm;
out vec4 vOutputColour;

uniform sampler2D sampler0;

void main() {
    vec4 baseTex = texture(sampler0, vTexCoord);
    const vec4 greenTint = vec4(0.2, 0.6, 0.2, 1.0);
    vec4 finalTex = baseTex * greenTint;
    const vec3 fakeLight = vec3(0.7, 0.7, 0.7); 
    vOutputColour = finalTex * vec4(fakeLight, 1.0);
    float dist = length(vEyePos.xyz);
    float fogFactor = clamp((200.0 - dist) / (200.0 - 50.0), 0.0, 1.0);
    vOutputColour = mix(vec4(0.5, 0.5, 0.5, 1.0), vOutputColour, fogFactor);
}