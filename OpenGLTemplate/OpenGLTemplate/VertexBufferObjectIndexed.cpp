// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified with custom logic.
// ---------------------------------------------------------
#include "VertexBufferObjectIndexed.h"

CVertexBufferObjectIndexed::CVertexBufferObjectIndexed() : m_vboVertices(0), m_vboIndices(0), m_dataUploaded(false)
{
}

CVertexBufferObjectIndexed::~CVertexBufferObjectIndexed()
{
	Release();
}

void CVertexBufferObjectIndexed::Create()
{
	glGenBuffers(1, &m_vboVertices);
	glGenBuffers(1, &m_vboIndices);
}

void CVertexBufferObjectIndexed::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
}

void CVertexBufferObjectIndexed::Release()
{
	if (m_vboVertices != 0) {
		glDeleteBuffers(1, &m_vboVertices);
		m_vboVertices = 0;
	}
	if (m_vboIndices != 0) {
		glDeleteBuffers(1, &m_vboIndices);
		m_vboIndices = 0;
	}

	m_dataUploaded = false;

	m_vertexData.clear();
	m_vertexData.shrink_to_fit();

	m_indexData.clear();
	m_indexData.shrink_to_fit();
}

void CVertexBufferObjectIndexed::AddVertexData(const void* ptrVertexData, GLuint uiVertexDataSize)
{
	const BYTE* pByteData = static_cast<const BYTE*>(ptrVertexData);
	m_vertexData.insert(m_vertexData.end(), pByteData, pByteData + uiVertexDataSize);
}

void CVertexBufferObjectIndexed::AddIndexData(const void* ptrIndexData, GLuint uiIndexDataSize)
{
	const BYTE* pByteData = static_cast<const BYTE*>(ptrIndexData);
	m_indexData.insert(m_indexData.end(), pByteData, pByteData + uiIndexDataSize);
}

void CVertexBufferObjectIndexed::UploadDataToGPU(GLenum usageHint)
{
	if (!m_vertexData.empty()) {
		glBufferData(GL_ARRAY_BUFFER, m_vertexData.size(), m_vertexData.data(), usageHint);
		m_vertexData.clear();
		m_vertexData.shrink_to_fit();
	}

	if (!m_indexData.empty()) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size(), m_indexData.data(), usageHint);
		m_indexData.clear();
		m_indexData.shrink_to_fit();
	}

	m_dataUploaded = true;
}