// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Procedural Vertex Displacement via sum of Gerstner Waves
// - Environment Mapping using World-Space Reflection Vectors (CubeMap)
// - Fresnel Effect approximation for dynamic reflection intensity
// - Depth-based colour interpolation (Shallow vs Deep water)
// - Specular sun highlights and distance fog blending
// ---------------------------------------------------------


#version 400 core
in vec3 vEyePos;
in vec3 vEyeNorm;
in float waterHeight;

out vec4 vOutputColour;

uniform samplerCube CubeMapTex;
uniform mat4 viewMatrix;
uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform float t; 

void main() {
    vec3 dX = dFdx(vEyePos);
    vec3 dY = dFdy(vEyePos);
    vec3 geoNormal = normalize(cross(dX, dY));
    if (dot(geoNormal, vEyePos) > 0.0) {
        geoNormal = -geoNormal;
    }
    float wave3 = sin(vEyePos.x * 0.15 - vEyePos.z * 0.1 + t * 1.2);
    vec3 normalOffset = vec3(wave3 * 0.02, 0.0, wave3 * 0.02);
    vec3 n = normalize(geoNormal + normalOffset);
    vec3 V = normalize(-vEyePos);          
    vec3 L = normalize(sunDirection);      
    float fresnel = 0.02 + 0.98 * pow(1.0 - max(dot(V, n), 0.0), 5.0);
    vec3 reflectDirEye = reflect(-V, n);
    vec3 reflectDirWorld = mat3(inverse(viewMatrix)) * reflectDirEye;
    vec4 reflectColor = texture(CubeMapTex, reflectDirWorld);
    vec3 H = normalize(L + V); 
    float specTarget = max(dot(n, H), 0.0);
    float specularLight = pow(specTarget, 60.0); 
    vec3 sunHighlight = sunColor * specularLight * 2.0; 
    const vec3 deepWater = vec3(0.01, 0.06, 0.18); 
    const vec3 shallowWater = vec3(0.05, 0.25, 0.35);
    float heightFactor = clamp((waterHeight + 10.0) / 25.0, 0.0, 1.0);
    vec3 waterBaseColor = mix(deepWater, shallowWater, heightFactor);
    vec3 finalColor = mix(waterBaseColor, reflectColor.rgb, fresnel);
    finalColor += sunHighlight;
    float dist = length(vEyePos);
    float fogFactor = clamp((1500.0 - dist) / (1000.0 - 200.0), 0.0, 1.0);
    const vec4 fogColor = vec4(0.85, 0.55, 0.65, 0.85);
    float baseAlpha = mix(0.95, 0.70, heightFactor);
    float finalAlpha = mix(baseAlpha, 0.95, fresnel);
    finalAlpha = min(finalAlpha + (specularLight * 2.0), 1.0);
    vOutputColour = mix(fogColor, vec4(finalColor, finalAlpha), fogFactor);
}