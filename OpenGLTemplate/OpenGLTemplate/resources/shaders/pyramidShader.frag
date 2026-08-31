// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Custom primitive rendering pipeline
// - Basic directional diffuse lighting calculations
// ---------------------------------------------------------


#version 400 core
in vec2 vTexCoord;
in vec3 vNormal;
uniform sampler2D sampler0;
out vec4 vOutputColour;

void main() {
	vec4 texColour = texture(sampler0, vTexCoord);
	vec3 normal = normalize(vNormal);
	const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); 
	float diffuseAmt = max(dot(normal, lightDir), 0.2);
	vOutputColour = vec4(texColour.rgb * diffuseAmt, texColour.a);
}