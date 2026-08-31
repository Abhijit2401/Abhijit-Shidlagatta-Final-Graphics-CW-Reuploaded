#pragma once

#include "Common.h"

// This class provides a wrapper around an OpenGL Vertex Buffer Object
class CVertexBufferObject
{
public:
	CVertexBufferObject();
	~CVertexBufferObject();

	void Create();
	void Bind();
	void Release();

	void AddData(const void* ptrData, GLuint dataSize);
	void UploadDataToGPU(GLenum usageHint);


private:
	GLuint m_vbo;
	vector<BYTE> m_data;
	bool m_dataUploaded;
};