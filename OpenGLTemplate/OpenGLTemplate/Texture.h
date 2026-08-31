#pragma once

#include "Common.h"

// Class that provides a texture for texture mapping in OpenGL
class CTexture
{
public:
	bool Load(const string& path, bool generateMipMaps = true);
	void CreateFromData(BYTE* data, int width, int height, int bpp, GLenum format, bool generateMipMaps = false);
	void Bind(int textureUnit = 0);
	void SetSamplerObjectParameter(GLenum parameter, GLenum value);
	void SetSamplerObjectParameterf(GLenum parameter, float value);
	int GetWidth() const;
	int GetHeight() const;
	int GetBPP() const;

	void Release();

	CTexture();
	~CTexture();
private:
	int m_width, m_height, m_bpp;
	GLuint m_textureID;
	GLuint m_samplerObjectID;
	bool m_mipMapsGenerated;

	string m_path;
};