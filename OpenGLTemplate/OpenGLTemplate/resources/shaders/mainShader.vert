// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// ---------------------------------------------------------
#version 400 core

uniform struct Matrices {
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

uniform mat4 inverseViewMatrix;
uniform mat4 lightSpaceMatrix;
uniform bool bIsTree;
uniform float t;
uniform float treePhase;

layout (location = 0) in vec3 inPosition; 
layout (location = 1) in vec2 inCoord;    
layout (location = 2) in vec3 inNormal;   

out vec3 vEyeNorm;     
out vec4 vEyePos;      
out vec2 vTexCoord;    
out vec3 worldPosition; 
out vec4 vFragPosLightSpace;

void main() {	
	vec3 localPos = inPosition;
	if (bIsTree) {
		float phase = treePhase * 0.1; 
		float swayStrength = max(0.0, localPos.y) * 0.03; 
		localPos.x += sin(t * 1.5 + phase) * swayStrength;
		localPos.z += cos(t * 1.1 + phase) * swayStrength;
	}
	worldPosition = localPos;
	vEyePos = matrices.modelViewMatrix * vec4(localPos, 1.0);
	vEyeNorm = matrices.normalMatrix * inNormal;
	vTexCoord = inCoord;
	vec4 wPos = inverseViewMatrix * vEyePos;
	vFragPosLightSpace = lightSpaceMatrix * wPos;
	gl_Position = matrices.projMatrix * vEyePos;
}