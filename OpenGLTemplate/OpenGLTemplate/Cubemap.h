// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "Texture.h"
#include "vertexBufferObject.h"
#include "./include/glm/gtc/type_ptr.hpp"

// Handles the 6 sided cube texture used for the skybox and car reflections
class CCubemap
{
public:
	// Loads the 6 individual images into one  cubemap
	void Create(string sPositiveX, string sNegativeX, string sPositiveY, string sNegativeY, string sPositiveZ, string sNegativeZ);

	// Binds the cubemap to a texture unit so the shader can use it
	void Bind(int iTextureUnit = 0);

	// Frees up GPU memory
	void Release();

private:
	// Helper to actually decode the image files
	bool LoadTexture(string filename, BYTE** bmpBytes, int& iWidth, int& iHeight);

	GLuint m_uiVAO;
	GLuint m_uiTexture;
	GLuint m_uiSampler;
};