// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
layout(location = 0) in vec3 inPosition;

struct Matrices {
    mat4 projMatrix;
    mat4 modelViewMatrix;
};
uniform Matrices matrices;

out vec3 worldPosition;

void main() {
    worldPosition = inPosition;
    gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);
}
