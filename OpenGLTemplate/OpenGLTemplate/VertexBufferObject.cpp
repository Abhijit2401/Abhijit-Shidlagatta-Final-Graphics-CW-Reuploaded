#include "VertexBufferObject.h"

CVertexBufferObject::CVertexBufferObject() : m_vbo(0), m_dataUploaded(false)
{
}

CVertexBufferObject::~CVertexBufferObject()
{
	Release();
}

void CVertexBufferObject::Create()
{
	glGenBuffers(1, &m_vbo);
}

void CVertexBufferObject::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void CVertexBufferObject::Release()
{
	if (m_vbo != 0) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
	m_dataUploaded = false;
	m_data.clear();
	m_data.shrink_to_fit();
}

void CVertexBufferObject::AddData(const void* ptrData, GLuint dataSize)
{
	const BYTE* pByteData = static_cast<const BYTE*>(ptrData);
	m_data.insert(m_data.end(), pByteData, pByteData + dataSize);
}

void CVertexBufferObject::UploadDataToGPU(GLenum usageHint)
{
	if (m_data.empty()) return;
	glBufferData(GL_ARRAY_BUFFER, m_data.size(), m_data.data(), usageHint);
	m_dataUploaded = true;
	m_data.clear();
	m_data.shrink_to_fit();
}