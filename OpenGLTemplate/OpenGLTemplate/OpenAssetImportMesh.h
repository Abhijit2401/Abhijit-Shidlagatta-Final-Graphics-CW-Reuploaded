/*
	Copyright 2011 Etay Meiri
	Modified by Dr Greg Slabaugh to work with OpenGL template
	Modified for AAA Standard & Optimization

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#pragma once

#include <map>
#include <vector>
#include "include/gl/glew.h"
#include <Importer.hpp>      
#include <scene.h>           
#include <postprocess.h>     

#include "Common.h"
#include "Texture.h"

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define INVALID_MATERIAL 0xFFFFFFFF

// Custom struct to hold exactly what the shader needs for a single vertex point
struct Vertex
{
	glm::vec3 m_pos;
	glm::vec2 m_tex;
	glm::vec3 m_normal;

	Vertex() {}
	Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
	{
		m_pos = pos;
		m_tex = tex;
		m_normal = normal;
	}
};

// Uses the assimp library to load complex .obj model files
class COpenAssetImportMesh
{
public:
	COpenAssetImportMesh();
	~COpenAssetImportMesh();

	bool Load(const std::string& Filename);
	void Render();

private:
	// Internal setup functions for the assimp library
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void Clear();

	struct MeshEntry {
		MeshEntry();
		~MeshEntry();

		void Init(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices);

		GLuint vbo;
		GLuint ibo;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<CTexture*> m_Textures;
	GLuint m_vao;
};