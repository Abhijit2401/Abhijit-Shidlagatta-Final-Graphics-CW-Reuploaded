#include "Common.h" 
#include "Plane.h"

// Ensures that the GetTerrainHeight function is available for use in this file
extern float GetTerrainHeight(float x, float z);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CPlane::CPlane() : m_numVertices(0) {}
CPlane::~CPlane() {}

void CPlane::Create(const string& directory, const string& filename1, const string& filename2, const string& filename3, const string& filename4, float width, float height, float textureRepeat)
{
	m_width = width;
	m_height = height;
	m_directory = directory;
	m_filename = filename1;

	// Loads the grass texture
	m_texture.Load(directory + filename1, true);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Loads the stone texture
	m_texture2.Load(directory + filename2, true);
	m_texture2.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture2.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture2.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture2.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Loads the snow texture
	m_texture3.Load(directory + filename3, true);
	m_texture3.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture3.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture3.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture3.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Loads the sand texture
	m_texture4.Load(directory + filename4, true);
	m_texture4.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture4.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture4.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture4.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	// 400x400 grid resolution
	int resolution = 400;
	float startX = -m_width / 2.0f;
	float startZ = -m_height / 2.0f;
	float stepX = m_width / (float)resolution;
	float stepZ = m_height / (float)resolution;

	// (width * height * 6 vertices per quad)
	m_numVertices = (GLsizei)(resolution * resolution * 6);

	// Gets the normal vector at a given vertex by sampling the height of the surrounding vertices
	auto getNorm = [&](float vx, float vz) {
		float hL = GetTerrainHeight(vx - 0.1f, vz);
		float hR = GetTerrainHeight(vx + 0.1f, vz);
		float hD = GetTerrainHeight(vx, vz - 0.1f);
		float hU = GetTerrainHeight(vx, vz + 0.1f);
		return glm::normalize(glm::vec3(hL - hR, 0.2f, hD - hU));
		};

	// build the 3d grid
	for (int z = 0; z < resolution; z++) {
		for (int x = 0; x < resolution; x++) {

			float x0 = startX + x * stepX;
			float z0 = startZ + z * stepZ;
			float x1 = startX + (x + 1) * stepX;
			float z1 = startZ + (z + 1) * stepZ;

			// Samples the y height directly from the math generator
			float y00 = GetTerrainHeight(x0, z0);
			float y10 = GetTerrainHeight(x1, z0);
			float y01 = GetTerrainHeight(x0, z1);
			float y11 = GetTerrainHeight(x1, z1);

			glm::vec3 p00(x0, y00, z0); glm::vec2 t00((float)x / resolution * textureRepeat, (float)z / resolution * textureRepeat); glm::vec3 n00 = getNorm(x0, z0);
			glm::vec3 p10(x1, y10, z0); glm::vec2 t10((float)(x + 1) / resolution * textureRepeat, (float)z / resolution * textureRepeat); glm::vec3 n10 = getNorm(x1, z0);
			glm::vec3 p01(x0, y01, z1); glm::vec2 t01((float)x / resolution * textureRepeat, (float)(z + 1) / resolution * textureRepeat); glm::vec3 n01 = getNorm(x0, z1);
			glm::vec3 p11(x1, y11, z1); glm::vec2 t11((float)(x + 1) / resolution * textureRepeat, (float)(z + 1) / resolution * textureRepeat); glm::vec3 n11 = getNorm(x1, z1);

			// tri 1
			m_vbo.AddData(&p00, sizeof(glm::vec3)); m_vbo.AddData(&t00, sizeof(glm::vec2)); m_vbo.AddData(&n00, sizeof(glm::vec3));
			m_vbo.AddData(&p01, sizeof(glm::vec3)); m_vbo.AddData(&t01, sizeof(glm::vec2)); m_vbo.AddData(&n01, sizeof(glm::vec3));
			m_vbo.AddData(&p10, sizeof(glm::vec3)); m_vbo.AddData(&t10, sizeof(glm::vec2)); m_vbo.AddData(&n10, sizeof(glm::vec3));

			// tri 2
			m_vbo.AddData(&p10, sizeof(glm::vec3)); m_vbo.AddData(&t10, sizeof(glm::vec2)); m_vbo.AddData(&n10, sizeof(glm::vec3));
			m_vbo.AddData(&p01, sizeof(glm::vec3)); m_vbo.AddData(&t01, sizeof(glm::vec2)); m_vbo.AddData(&n01, sizeof(glm::vec3));
			m_vbo.AddData(&p11, sizeof(glm::vec3)); m_vbo.AddData(&t11, sizeof(glm::vec2)); m_vbo.AddData(&n11, sizeof(glm::vec3));
		}
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CPlane::Render()
{
	glBindVertexArray(m_vao);

	// Binds all 4 textures so the shader can blend them based on height
	m_texture.Bind(0);
	m_texture2.Bind(1);
	m_texture3.Bind(2);
	m_texture4.Bind(3);

	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

void CPlane::Release()
{
	// Releases all 4 textures so VARM doesnt leak memory
	m_texture.Release();
	m_texture2.Release();
	m_texture3.Release();
	m_texture4.Release();

	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}