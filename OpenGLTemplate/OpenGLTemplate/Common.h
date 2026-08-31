// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// Major logic written/edited by me:
// - Centralised Game Configuration Constants (Spawn rates, Hitboxes)
// - Procedural GetTerrainHeight() math using exponential mountain peaks
// - Combination of Sine/Cosine waves for organic high-frequency noise
// ---------------------------------------------------------
#pragma once

#include <ctime>
#include <windows.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm> 
#include "include/glm/gtc/type_ptr.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtx/rotate_vector.hpp"
#include "include/gl/glew.h"
#include <gl/gl.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

// Global terrain math so everything knows how high the ground is
inline float GetTerrainHeight(float x, float z)
{
	float distSq = (x * x) + (z * z);
	float distCenter = sqrt(distSq);

	// Exponential for 3 mountains
	float dx1 = x + 250.0f;
	float dz1 = z + 250.0f;
	float peak1 = 500.0f * exp(-sqrt((dx1 * dx1) + (dz1 * dz1)) / 300.0f);

	float dx2 = x - 300.0f;
	float dz2 = z;
	float peak2 = 580.0f * exp(-sqrt((dx2 * dx2) + (dz2 * dz2)) / 350.0f);

	float dx3 = x;
	float dz3 = z - 350.0f;
	float peak3 = 540.0f * exp(-sqrt((dx3 * dx3) + (dz3 * dz3)) / 320.0f);

	float mountain = std::max({ peak1, peak2, peak3 });

	// gaussian bell curve for the main island base
	float islandBase = 15.0f * exp(-distSq / 4000000.0f);

	// Sine waves mixed together to make organic looking hills
	float ringDistance = distCenter - 800.0f;
	float ringMask = exp(-(ringDistance * ringDistance) / 62500.0f);

	float hillWave1 = sin(x * 0.015f + z * 0.01f);
	float hillWave2 = cos(-x * 0.01f + z * 0.015f);
	float hillWave3 = sin(x * 0.03f - z * 0.02f) * 0.5f;

	float organicHills = (hillWave1 + hillWave2 + hillWave3 + 1.5f) * 25.0f;
	float hills = organicHills * ringMask;

	float finalHeight = mountain + islandBase + hills;

	// Clamps so the terrain doesnt dip under the flat water plane
	if (finalHeight < 40.0f) {
		finalHeight = 40.0f - ((40.0f - finalHeight) * 3.0f);
	}

	return finalHeight;
}