// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - World-space reflection vector calculation (R = reflect(V, N))
// - Cubemap environment sampling and base texture compositing
// ---------------------------------------------------------

#version 400 core

in vec3 worldPos;
in vec3 worldNormal;
in vec2 texCoord; 

uniform vec3 cameraPos;
uniform samplerCube CubeMapTex;
uniform sampler2D sampler0;

out vec4 outputColor;

void main() 
{
    vec3 N = normalize(worldNormal);
    vec3 V = normalize(worldPos - cameraPos);
    vec3 R = reflect(V, N);
    vec4 baseColor = texture(sampler0, texCoord);
    vec4 reflectionColor = texture(CubeMapTex, R);
    outputColor = mix(baseColor, reflectionColor, 0.1);
}