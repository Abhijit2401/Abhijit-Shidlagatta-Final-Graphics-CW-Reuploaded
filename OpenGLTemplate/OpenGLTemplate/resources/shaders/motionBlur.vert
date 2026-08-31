// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = inCoord;
	gl_Position = vec4(inPosition, 1.0); // Screen-space quad
}