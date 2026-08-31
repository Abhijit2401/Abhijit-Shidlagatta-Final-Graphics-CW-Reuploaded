#version 400 core

in vec2 vTexCoord;
out vec4 vOutputColour;

uniform sampler2D sampler0;
uniform vec4 vColour;

void main()
{
	vec4 vTexColour = texture(sampler0, vTexCoord);	
	vOutputColour = vec4(vTexColour.r) * vColour;			
}