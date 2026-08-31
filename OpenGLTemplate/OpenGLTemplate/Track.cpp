// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// Major logic written by me:
// - Procedural track generation using inner/outer radii mathematical boundaries
// - Trigonometric vertex distribution mapped to GL_TRIANGLE_STRIP
// - Dynamic UV coordinate scaling based on segment length
// ---------------------------------------------------------
#include "Common.h"
#include "Track.h"

#define _USE_MATH_DEFINES
#include <math.h>

CTrack::CTrack() : m_vao(0), m_numVertices(0) {}
CTrack::~CTrack() {}

void CTrack::Create(const string& directory, const string& filename, float innerRadius, float outerRadius, int segments)
{
	m_numVertices = (GLsizei)((segments + 1) * 2);
	// Loads the texture for the track
	m_texture.Load(directory + filename, true);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	// Connects the xyz position, uv texture coordinates, and normal vectors to the vertex buffer object
	for (int i = 0; i <= segments; i++) {
		// Calculates the angle for the current segment
		float angle = (float)i / (float)segments * 2.0f * (float)M_PI;
		float x = cos(angle);
		float z = sin(angle);

		glm::vec3 normal(0.0f, 1.0f, 0.0f);

		// Outer edge of the track
		glm::vec3 outerPos(-20.0f + x * outerRadius, 0.05f, z * outerRadius);
		glm::vec2 outerTex((float)i / segments, 1.0f);
		m_vbo.AddData(&outerPos, sizeof(glm::vec3));
		m_vbo.AddData(&outerTex, sizeof(glm::vec2));
		m_vbo.AddData(&normal, sizeof(glm::vec3));

		// Inner edge of the track
		glm::vec3 innerPos(-20.0f + x * innerRadius, 0.05f, z * innerRadius);
		glm::vec2 innerTex((float)i / segments, 0.0f);
		m_vbo.AddData(&innerPos, sizeof(glm::vec3));
		m_vbo.AddData(&innerTex, sizeof(glm::vec2));
		m_vbo.AddData(&normal, sizeof(glm::vec3));
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Connects the vertex attributes to the shader program
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Connects the texture coordinates to the shader program
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Connects the normal vectors to the shader program
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CTrack::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numVertices);
}

void CTrack::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}