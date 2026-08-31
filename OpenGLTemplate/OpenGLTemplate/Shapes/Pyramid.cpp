// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Procedural 3D square-based pyramid geometry calculation
// - Manual surface normal mathematics for accurate lighting
// - Hardware Instancing support via glDrawArraysInstanced (used for particles)
// ---------------------------------------------------------
#include "../Common.h"
#include "Pyramid.h"

CPyramid::CPyramid() : m_size(0.0f), m_vao(0) {}
CPyramid::~CPyramid() {}

void CPyramid::Create(const string& directory, const string& filename, float size)
{
	m_size = size;
	m_directory = directory;
	m_filename = filename;

	// Loads the texture
	m_texture.Load(directory + filename, true);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	float s = m_size / 2.0f;

	// Points for the pyramid
	glm::vec3 tip(0.0f, s, 0.0f);
	glm::vec3 frontLeft(-s, 0.0f, s);
	glm::vec3 frontRight(s, 0.0f, s);
	glm::vec3 backRight(s, 0.0f, -s);
	glm::vec3 backLeft(-s, 0.0f, -s);

	glm::vec3 pyramidVertices[18] = {
		tip, frontLeft, frontRight,
		tip, frontRight, backRight,
		tip, backRight, backLeft,
		tip, backLeft, frontLeft,
		frontLeft, backLeft, backRight,
		frontLeft, backRight, frontRight
	};

	glm::vec2 texCoords[18] = {
		glm::vec2(0.5f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.5f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.5f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.5f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)
	};

	//Approximate normals pointing outwards so lighting hits it correctly
	float normY = 2.0f / sqrt(5.0f);
	float normXZ = 1.0f / sqrt(5.0f);

	glm::vec3 nFront(0.0f, normY, normXZ);
	glm::vec3 nRight(normXZ, normY, 0.0f);
	glm::vec3 nBack(0.0f, normY, -normXZ);
	glm::vec3 nLeft(-normXZ, normY, 0.0f);
	glm::vec3 nDown(0.0f, -1.0f, 0.0f);

	glm::vec3 pyramidNormals[18] = {
		nFront, nFront, nFront,
		nRight, nRight, nRight,
		nBack, nBack, nBack,
		nLeft, nLeft, nLeft,
		nDown, nDown, nDown,
		nDown, nDown, nDown
	};

	// Puts into VBO
	for (int i = 0; i < 18; i++) {
		m_vbo.AddData(&pyramidVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&texCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&pyramidNormals[i], sizeof(glm::vec3));
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CPyramid::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 18);
}

void CPyramid::RenderInstanced(int instanceCount)
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 18, (GLsizei)instanceCount);
}

void CPyramid::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}