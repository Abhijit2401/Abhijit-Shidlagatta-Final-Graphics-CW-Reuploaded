// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#pragma once

#include "../Common.h"
#include "../Texture.h"
#include "../VertexBufferObject.h"

// Generates 3d cylinder mesh with given radius, height and number of slices
class CCylinder
{
public:
	CCylinder();
	~CCylinder();

	void Create(const string& directory, const string& filename, float radius, float height, int slices);
	void Render();
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	GLsizei m_numTubeVertices;  // vertex count for the main body
	GLsizei m_numCapVertices;   // vertex count for the caps
};