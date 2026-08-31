// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
out vec4 vOutputColour;

uniform vec3 outlineColour;

void main() {
    vOutputColour = vec4(outlineColour, 1.0);
}
