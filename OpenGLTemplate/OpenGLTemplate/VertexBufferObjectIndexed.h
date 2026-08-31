// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include "Common.h"

class CVertexBufferObjectIndexed
{
public:
	CVertexBufferObjectIndexed();
	~CVertexBufferObjectIndexed();

	void Create();									// Creates a VBO
	void Bind();									// Binds the VBO
	void Release();									// Releases the VBO

	void AddVertexData(const void* pVertexData, GLuint vertexDataSize); // Adds vertex data
	void AddIndexData(const void* pIndexData, GLuint indexDataSize);	// Adds index data
	void UploadDataToGPU(GLenum usageHint);			// Upload the VBO to the GPU

private:
	GLuint m_vboVertices;		// VBO id for vertices
	GLuint m_vboIndices;		// VBO id for indices

	vector<BYTE> m_vertexData;	// Vertex data to be uploaded
	vector<BYTE> m_indexData;	// Index data to be uploaded

	bool m_dataUploaded;		// Flag indicating if data is uploaded to the GPU
};