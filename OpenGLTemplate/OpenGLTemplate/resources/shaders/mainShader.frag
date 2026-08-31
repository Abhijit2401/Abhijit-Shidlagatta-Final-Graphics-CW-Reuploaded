// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// Major logic written/edited by me:
// - Procedural Wind Animation (Trigonometric vertex sway for trees)
// - Dynamic Blinn-Phong Spotlight (Vehicle Headlights)
// - Shadow Mapping with 3x3 Percentage-Closer Filtering (PCF)
// - Procedural Height & Slope-based Multi-Texturing (Sand, Grass, Rock, Snow)
// - Distance and Height-based Fog integration
// ---------------------------------------------------------
#version 400 core

in vec2 vTexCoord;
in vec4 vEyePos;
in vec3 vEyeNorm;
in vec3 worldPosition;
in vec4 vFragPosLightSpace;

out vec4 vOutputColour;

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2D sampler3;
uniform samplerCube CubeMapTex;
uniform sampler2D shadowMap; 
uniform bool bUseTexture;
uniform bool renderSkybox;
uniform bool bMultiTexture;
uniform bool bTerrainMode;
uniform bool bCelShading;
uniform float objectAlpha;
uniform float t;

struct LightInfo {
	vec4 position;
	vec3 direction;
	float exponent;
    float cutoff;
	vec3 La, Ld, Ls;
};
struct MaterialInfo {
	vec3 Ma, Md, Ms;
	float shininess;
};
uniform LightInfo light1; 
uniform MaterialInfo material1;
uniform vec3 sunDirection; 
uniform vec3 sunColor;

vec3 BlinnPhongSpotlightModel(vec4 p, vec3 n) {
    vec3 s = normalize(vec3(light1.position - p));
    float angle = acos(dot(-s, light1.direction));
    float cutoff = radians(clamp(light1.cutoff, 0.0, 90.0));
    vec3 ambient = light1.La * material1.Ma;
    if (angle < cutoff) {
        float spotFactor = pow(dot(-s, light1.direction), light1.exponent);
        vec3 v = normalize(-p.xyz);
        vec3 h = normalize(v + s);
        float sDotN = max(dot(s, n), 0.0);
        vec3 diffuse = light1.Ld * material1.Md * sDotN;
        vec3 specular = vec3(0.0);
        if (sDotN > 0.0) {
            specular = light1.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
        }
        return ambient + spotFactor * (diffuse + specular);
    } 
    return ambient;
}

float ShadowCalculation(vec4 fragPosLightSpace, float nDotL) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    if(projCoords.z > 1.0) return 0.0;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - nDotL), 0.001); 
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

// Quantises a 0-1 lighting value into discrete bands for a cel-shaded look
float CelBand(float value) {
    const float bands = 4.0;
    return ceil(value * bands) / bands;
}

void main() {
    if (objectAlpha > 0.98 && objectAlpha < 1.0) {
        float scanline = sin(worldPosition.y * 5.0 - (worldPosition.x * 2.0)); 
        if (scanline < 0.0) discard; 
    }
	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);
	} else {
		vec3 n = normalize(vEyeNorm);
		vec3 headlightLighting = BlinnPhongSpotlightModel(vEyePos, n);
		float nDotL = max(dot(n, normalize(sunDirection)), 0.0);
		float shadow = ShadowCalculation(vFragPosLightSpace, nDotL);
		float litAmount = bCelShading ? CelBand(nDotL) : nDotL;
		vec3 sunDiffuse = sunColor * litAmount * 0.8 * (1.0 - shadow);
		vec3 sunAmbient = sunColor * 0.2;
		vec3 finalLighting = headlightLighting + sunDiffuse + sunAmbient;
		if (bCelShading) {
			float rim = 1.0 - max(dot(n, normalize(-vEyePos.xyz)), 0.0);
			finalLighting += material1.Md * smoothstep(0.6, 1.0, rim);
		}
		vec4 vTexColour = texture(sampler0, vTexCoord);
		if (bTerrainMode) {
			vec4 grass = texture(sampler0, vTexCoord);
			vec4 stone = texture(sampler1, vTexCoord);
			vec4 snow  = texture(sampler2, vTexCoord); 
			vec4 sand  = texture(sampler3, vTexCoord); 
            float h = worldPosition.y; 
            vec4 terrainColor;
            if (h < 40.0) terrainColor = sand; 
            else if (h < 50.0)  terrainColor = mix(sand, grass, (h - 40.0) / 10.0); 
            else if (h < 180.0) terrainColor = mix(grass, stone, (h - 50.0) / 130.0); 
            else if (h < 250.0) terrainColor = mix(stone, snow, (h - 180.0) / 70.0); 
            else terrainColor = snow; 
			vec3 dX = dFdx(worldPosition);
			vec3 dY = dFdy(worldPosition);
			vec3 worldNorm = normalize(cross(dX, dY));
			float slope = abs(worldNorm.y); 
			float rockBlend = smoothstep(0.70, 0.85, slope);
			vTexColour = mix(stone, terrainColor, rockBlend);
		}
		else if (bMultiTexture) {
			vec4 vTexColour2 = texture(sampler1, vTexCoord);
			float edgeDist = abs(vTexCoord.x - 0.5) * 2.0; 
			float blendFactor = smoothstep(0.5, 0.9, edgeDist); 
			vTexColour = mix(vTexColour, vTexColour2, blendFactor);
		}
		if (bUseTexture) {
			vOutputColour = vTexColour * vec4(finalLighting, objectAlpha); 
		} else {
			vOutputColour = vec4(finalLighting, objectAlpha); 
		}
	}

    float dist = length(vEyePos.xyz);
    float fogMinDistance = 300.0; 
    float fogMaxDistance = 1500.0; 
	float distFogFactor = clamp((fogMaxDistance - dist) / (fogMaxDistance - fogMinDistance), 0.0, 1.0);
    float heightClearFactor = smoothstep(20.0, 70.0, worldPosition.y);
    float finalFogFactor = max(distFogFactor, heightClearFactor);
    const vec4 fogColor = vec4(0.85, 0.55, 0.65, 1.0); // AAA: const fog
    vOutputColour = mix(fogColor, vOutputColour, finalFogFactor);
}