#pragma once

#include "../Common.h"
#include "../Texture.h"
#include "../VertexBufferObjectIndexed.h"

// Class for generating a unit sphere
class CSphere
{
public:
	CSphere();
	~CSphere();
	void Create(const string& directory, const string& filename, int slicesIn, int stacksIn);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	GLsizei m_numTriangles;
};