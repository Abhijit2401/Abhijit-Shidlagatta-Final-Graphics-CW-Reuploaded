// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoord;
layout(location = 2) in vec3 inNormal;

struct Matrices {
    mat4 projMatrix;
    mat4 modelViewMatrix;
    mat3 normalMatrix;
};
uniform Matrices matrices;
uniform float t;

out vec3 vEyePos;
out vec3 vEyeNorm;
out float waterHeight; 
vec3 GerstnerWave(vec2 dir, float steepness, float wavelength, float speed, float time, vec3 p) {
    float k = 2.0 * 3.141 / wavelength;
    float c = sqrt(9.8 / k) * speed; 
    vec2 d = normalize(dir);
    float f = k * (dot(d, p.xz) - c * time);
    float a = steepness / k;
    return vec3(
        d.x * (a * cos(f)),
        a * sin(f),
        d.y * (a * cos(f))
    );
}

void main() {
    vec3 displacedPos = inPosition;
    displacedPos += GerstnerWave(vec2(1.0, 0.5), 0.2, 500.0, 1.5, t, inPosition);
    displacedPos += GerstnerWave(vec2(0.7, -0.6), 0.1, 300.0, 1.4, t, inPosition);
    waterHeight = displacedPos.y; 
    vec4 eyePos = matrices.modelViewMatrix * vec4(displacedPos, 1.0);
    vEyePos = eyePos.xyz;
    vEyeNorm = normalize(matrices.normalMatrix * inNormal);
    gl_Position = matrices.projMatrix * eyePos;
}