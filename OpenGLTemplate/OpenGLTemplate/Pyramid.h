// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

//class for generating a 3D Pyramid with a Square Base
class CPyramid
{
public:
	CPyramid();
	~CPyramid();
	void Create(const string& directory, const string& filename, float size);
	void Render();
	void RenderInstanced(int instanceCount);
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	float m_size;
};