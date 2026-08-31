// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - HDR Brightness Extraction using Rec. 709 Biological Luminance coefficients
// - 8-Directional, 24-sample expanding Gaussian blur
// - Additive Scene Compositing for final Bloom effect
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D baseTexture;    
uniform sampler2D brightTexture;  

void main() {
    vec2 tex_offset = 1.0 / vec2(textureSize(brightTexture, 0)); 
    vec3 bloom = texture(brightTexture, vTexCoord).rgb;
    float spread = 3.0; 
    const vec2 offsets[8] = vec2[](
        vec2(1.0, 0.0), vec2(-1.0, 0.0), vec2(0.0, 1.0), vec2(0.0, -1.0),
        vec2(1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0)
    );
    for(int i = 0; i < 8; i++) {
        bloom += texture(brightTexture, vTexCoord + offsets[i] * tex_offset * spread).rgb * 0.5;
        bloom += texture(brightTexture, vTexCoord + offsets[i] * tex_offset * spread * 2.0).rgb * 0.25;
        bloom += texture(brightTexture, vTexCoord + offsets[i] * tex_offset * spread * 3.0).rgb * 0.125;
    }
    vec3 baseColor = texture(baseTexture, vTexCoord).rgb;
    FragColor = vec4(baseColor + (bloom * 0.4), 1.0);
}