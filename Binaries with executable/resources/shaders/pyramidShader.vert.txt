// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

uniform struct Matrices {
	mat4 projMatrix;
	mat4 modelViewMatrix;
	mat3 normalMatrix;
} matrices;

out vec2 vTexCoord;
out vec3 vNormal;

void main() {
	vTexCoord = inTexCoord;
	vNormal = matrices.normalMatrix * inNormal;
	vec4 eyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0);
	gl_Position = matrices.projMatrix * eyePosition;
}