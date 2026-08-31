// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Procedural 3D cylinder mesh generation via trigonometry (sin/cos)
// - Dynamic vertex, normal, and UV coordinate calculation
// - Multi-draw call handling (GL_TRIANGLE_STRIP & GL_TRIANGLE_FAN)
// ---------------------------------------------------------
#include "../Common.h"
#include "CCylinder.h"
#define _USE_MATH_DEFINES
#include <math.h>

CCylinder::CCylinder() {}
CCylinder::~CCylinder() {}

void CCylinder::Create(const string& directory, const string& filename, float radius, float height, int slices) {

	m_texture.Load(directory + filename, true);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	float halfHeight = height / 2.0f;

	// Calculates vertex counts for draw calls
	m_numTubeVertices = (GLsizei)((slices + 1) * 2);
	m_numCapVertices = (GLsizei)(slices + 2);

	for (int i = 0; i <= slices; i++) {
		float theta = ((float)i / slices) * 2.0f * (float)M_PI;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		float u = (float)i / slices;

		glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

		glm::vec3 bottomPos(x, -halfHeight, z);
		glm::vec2 bottomTex(u, 0.0f);
		m_vbo.AddData(&bottomPos, sizeof(glm::vec3));
		m_vbo.AddData(&bottomTex, sizeof(glm::vec2));
		m_vbo.AddData(&normal, sizeof(glm::vec3));

		glm::vec3 topPos(x, halfHeight, z);
		glm::vec2 topTex(u, 1.0f);
		m_vbo.AddData(&topPos, sizeof(glm::vec3));
		m_vbo.AddData(&topTex, sizeof(glm::vec2));
		m_vbo.AddData(&normal, sizeof(glm::vec3));
	}

	// Draws top
	glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
	glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
	glm::vec2 topCenterTex(0.5f, 0.5f);

	m_vbo.AddData(&topCenter, sizeof(glm::vec3));
	m_vbo.AddData(&topCenterTex, sizeof(glm::vec2));
	m_vbo.AddData(&topNormal, sizeof(glm::vec3));

	for (int i = slices; i >= 0; i--) {
		float theta = ((float)i / slices) * 2.0f * (float)M_PI;
		float x = radius * cos(theta);
		float z = radius * sin(theta);

		glm::vec3 pos(x, halfHeight, z);
		glm::vec2 tex((cos(theta) + 1.0f) * 0.5f, (sin(theta) + 1.0f) * 0.5f);

		m_vbo.AddData(&pos, sizeof(glm::vec3));
		m_vbo.AddData(&tex, sizeof(glm::vec2));
		m_vbo.AddData(&topNormal, sizeof(glm::vec3));
	}

	// drawing the bottom
	glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
	glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
	glm::vec2 bottomCenterTex(0.5f, 0.5f);

	m_vbo.AddData(&bottomCenter, sizeof(glm::vec3));
	m_vbo.AddData(&bottomCenterTex, sizeof(glm::vec2));
	m_vbo.AddData(&bottomNormal, sizeof(glm::vec3));

	for (int i = 0; i <= slices; i++) {
		float theta = ((float)i / slices) * 2.0f * (float)M_PI;
		float x = radius * cos(theta);
		float z = radius * sin(theta);

		glm::vec3 pos(x, -halfHeight, z);
		glm::vec2 tex((cos(theta) + 1.0f) * 0.5f, (sin(theta) + 1.0f) * 0.5f);

		m_vbo.AddData(&pos, sizeof(glm::vec3));
		m_vbo.AddData(&tex, sizeof(glm::vec2));
		m_vbo.AddData(&bottomNormal, sizeof(glm::vec3));
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

void CCylinder::Render() {
	glBindVertexArray(m_vao);
	m_texture.Bind();

	// Draw the cylinder body (tube) using GL_TRIANGLE_STRIP
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numTubeVertices);
	glDrawArrays(GL_TRIANGLE_FAN, (GLint)m_numTubeVertices, m_numCapVertices);
	glDrawArrays(GL_TRIANGLE_FAN, (GLint)(m_numTubeVertices + m_numCapVertices), m_numCapVertices);
}

void CCylinder::Release() {
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}