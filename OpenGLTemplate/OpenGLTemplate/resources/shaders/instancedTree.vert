// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core

uniform struct Matrices {
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vEyeNorm;
out vec4 vEyePos;
out vec2 vTexCoord;

void main() {	
	float x = (gl_InstanceID % 32) * 70.0 - 300.0;
	float z = (gl_InstanceID / 32) * 70.0 - 1100.0;
	vec3 shift = vec3(x, -15.0, z);
	vec3 stretchedPos = inPosition * vec3(1.2, 4.5, 1.2);
	float distFromCenter = length(vec2(x - 750.0, z));
	if (distFromCenter > 660.0 && distFromCenter < 880.0) {
		stretchedPos = vec3(0.0);
	}
	vec3 worldPos = stretchedPos + shift;
	vEyePos = matrices.modelViewMatrix * vec4(worldPos, 1.0);
	vEyeNorm = matrices.normalMatrix * inNormal;
	vTexCoord = inCoord;
	gl_Position = matrices.projMatrix * vEyePos;
}