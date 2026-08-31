#version 400 core

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix;
} matrices;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inCoord;

out vec2 vTexCoord;

void main()
{
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 0.0, 1.0);
	vTexCoord = inCoord;
}