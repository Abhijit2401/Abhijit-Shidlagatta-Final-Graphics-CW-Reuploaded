// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
in vec3 worldPosition;
out vec4 vOutputColour;

uniform vec3 playerPos;
uniform vec3 edgeColor;
uniform float t;

// Fades from invisible to a bright pulsing glow as the player gets close, so the
// boundary only reads as a wall right when it actually matters
void main() {
    float dist = length(worldPosition - playerPos);
    float proximity = 1.0 - smoothstep(5.0, 16.0, dist);
    float pulse = 0.85 + 0.15 * sin(t * 6.0);
    vOutputColour = vec4(edgeColor * proximity * pulse * 1.2, proximity * 0.55);
}
