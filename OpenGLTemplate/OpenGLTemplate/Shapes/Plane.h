#pragma once

#include "../Texture.h"
#include "../VertexBufferObject.h"
#include "../Common.h"

// Plane class that can be used to render a flat surface with a texture applied to it
class CPlane
{
public:
	CPlane();
	~CPlane();

	// string passed by const ref for speed
	void Create(const string& sDirectory, const string& sFilename1, const string& sFilename2, const string& sFilename3, const string& sFilename4, float fWidth, float fHeight, float fTextureRepeat);
	void Render();
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	CTexture m_texture2;
	CTexture m_texture3;
	CTexture m_texture4;
	string m_directory;
	string m_filename;
	float m_width;
	float m_height;
	GLsizei m_numVertices;
};