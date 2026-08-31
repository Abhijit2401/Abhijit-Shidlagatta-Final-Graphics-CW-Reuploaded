// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D sceneTexture;

void main() {
    vec4 color = texture(sceneTexture, vTexCoord);

    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

if (brightness > 0.75) { 
        FragColor = color;
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); 
    }
}