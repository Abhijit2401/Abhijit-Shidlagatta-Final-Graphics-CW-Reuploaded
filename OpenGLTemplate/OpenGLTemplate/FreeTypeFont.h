// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified
// ---------------------------------------------------------
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Common.h"
#include "Texture.h"
#include "Shaders.h"
#include "VertexBufferObject.h"

// 2D text renderer
class CFreeTypeFont
{
public:
	CFreeTypeFont();
	~CFreeTypeFont();

	bool LoadFont(string file, int pixelSize);
	bool LoadSystemFont(string name, int pixelSize);
	void ReleaseFont();
	void SetShaderProgram(CShaderProgram* shaderProgram);
	void Print(string text, int x, int y, int pixelSize = -1);
	void Render(int x, int y, int pixelSize, const char* text, ...);

	int GetTextWidth(string text, int pixelSize);

private:
	// Generates the texture for a single letter/character
	void CreateChar(int index);
	FT_Library m_ftLib;
	FT_Face m_ftFace;

	// Stores the textures and spacing info for the standard 256 ASCII characters
	CTexture m_charTextures[256];
	int m_advX[256], m_advY[256];
	int m_bearingX[256], m_bearingY[256];
	int m_charWidth[256], m_charHeight[256];
	int m_loadedPixelSize;
	int m_newLine;
	bool m_isLoaded;
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CShaderProgram* m_shaderProgram;
};