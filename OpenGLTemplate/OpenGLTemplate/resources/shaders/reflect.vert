// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord; 
layout (location = 2) in vec3 inNormal;  

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 texCoord;     

void main() 
{
    vec4 wPos = modelMatrix * vec4(inPosition, 1.0);
    worldPos = wPos.xyz;
    worldNormal = mat3(modelMatrix) * inNormal;
    texCoord = inCoord;
    gl_Position = projMatrix * viewMatrix * wPos;
}