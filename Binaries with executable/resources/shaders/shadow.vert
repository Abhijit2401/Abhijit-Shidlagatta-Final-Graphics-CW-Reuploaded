// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Depth-map generation pass for FBO
// - Orthographic light-space vertex transformation
// ---------------------------------------------------------

#version 400 core

layout (location = 0) in vec3 inPosition;

uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(inPosition, 1.0);
}