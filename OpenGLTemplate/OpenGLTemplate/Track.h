// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class CTrack {
public:
	CTrack();
	~CTrack();
	void Create(const string& directory, const string& filename, float innerRadius, float outerRadius, int segments);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	GLsizei m_numVertices;
};