// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vEyeNorm;
out vec4 vEyePos;
out vec2 vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float t;

void main() {	
    vec3 animatedPos = inPosition;
    if (inCoord.y > 0.1) {
        animatedPos.x += sin(t * 2.0 + modelMatrix[3][0]) * 0.12;
        animatedPos.z += cos(t * 1.5 + modelMatrix[3][2]) * 0.12;
    }
    vec4 worldPos = modelMatrix * vec4(animatedPos, 1.0);
    vEyePos = viewMatrix * worldPos;
    vEyeNorm = mat3(viewMatrix * modelMatrix) * inNormal;
    vTexCoord = inCoord;
    
    gl_Position = projMatrix * vEyePos;
}