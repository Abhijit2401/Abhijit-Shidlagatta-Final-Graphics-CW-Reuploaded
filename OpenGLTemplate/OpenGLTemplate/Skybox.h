// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
#include "Cubemap.h"

// This is a class for creating and rendering a skybox
class CSkybox
{
public:
	CSkybox();
	~CSkybox();
	void Create(float size);
	void Render(int textureUnit);
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CCubemap m_cubemapTexture;

};