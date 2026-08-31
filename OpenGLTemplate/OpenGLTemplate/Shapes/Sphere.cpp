#include "../Common.h"
#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CSphere::CSphere() : m_vao(0), m_numTriangles(0) {}
CSphere::~CSphere() {}

void CSphere::Create(const string& directory, const string& filename, int slicesIn, int stacksIn)
{
	m_directory = directory;
	m_filename = filename;

	m_texture.Load(directory + filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	for (int stacks = 0; stacks < stacksIn; stacks++) {
		// phi is the vertical angle (latitude)
		float phi = (stacks / (float)(stacksIn - 1)) * (float)M_PI;

		for (int slices = 0; slices <= slicesIn; slices++) {
			// theta is the horizontal angle around the equator (longitude)
			float theta = ((float)slices / slicesIn) * 2.0f * (float)M_PI;

			// Converts spherical angles to exact x, y, z positions
			glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
			glm::vec2 t = glm::vec2((float)slices / slicesIn, (float)stacks / stacksIn);
			glm::vec3 n = v;

			m_vbo.AddVertexData(&v, sizeof(glm::vec3));
			m_vbo.AddVertexData(&t, sizeof(glm::vec2));
			m_vbo.AddVertexData(&n, sizeof(glm::vec3));
		}
	}

	// Connects  the dots to make triangles
	m_numTriangles = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		for (int slices = 0; slices < slicesIn; slices++) {
			unsigned int nextSlice = slices + 1;

			// Wrap around the math to seal the sphere closed
			unsigned int nextStack = (stacks + 1) % stacksIn;

			// Grab the 4 corners of the current quad we are looking at
			unsigned int index0 = stacks * (slicesIn + 1) + slices;
			unsigned int index1 = nextStack * (slicesIn + 1) + slices;
			unsigned int index2 = stacks * (slicesIn + 1) + nextSlice;
			unsigned int index3 = nextStack * (slicesIn + 1) + nextSlice;

			// Triangle 1
			m_vbo.AddIndexData(&index0, sizeof(unsigned int));
			m_vbo.AddIndexData(&index1, sizeof(unsigned int));
			m_vbo.AddIndexData(&index2, sizeof(unsigned int));
			m_numTriangles++;

			// Triangle 2
			m_vbo.AddIndexData(&index2, sizeof(unsigned int));
			m_vbo.AddIndexData(&index1, sizeof(unsigned int));
			m_vbo.AddIndexData(&index3, sizeof(unsigned int));
			m_numTriangles++;
		}
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

void CSphere::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();

	// numTriangles * 3 gives us the exact index count for the draw call
	glDrawElements(GL_TRIANGLES, m_numTriangles * 3, GL_UNSIGNED_INT, 0);
}

void CSphere::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}