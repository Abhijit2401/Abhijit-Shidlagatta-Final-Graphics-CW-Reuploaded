// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float outlineThickness;

// Inflates the mesh along its normals so only the enlarged back faces poke out from behind the real model
void main() {
    vec3 inflatedPos = inPosition + normalize(inNormal) * outlineThickness;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(inflatedPos, 1.0);
}
