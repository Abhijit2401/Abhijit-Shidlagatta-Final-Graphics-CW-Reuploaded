// ---------------------------------------------------------
// Author: Abhijit Shidlagatta (210012420)
// Status: 100% original code (No external code copied).
// ---------------------------------------------------------
#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"

class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();

	// Generates the textured track
	void CreateTrack(string directory, string filename1, string filename2);
	void RenderTrack();

	int CurrentLap(float d);
	bool Sample(float d, glm::vec3& p, glm::vec3& up = _dummy_vector);

private:
	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);

	// Cubic spline math
	glm::vec3 Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t);

	vector<float> m_distances;			// Arc length map so speed stays constant
	CTexture m_texture;
	CTexture m_texture2;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;

	static glm::vec3 _dummy_vector;

	vector<glm::vec3> m_controlPoints;		// raw points from the text file
	vector<glm::vec3> m_controlUpVectors;	// terrain normals

	vector<glm::vec3> m_centrelinePoints;	// Smooth interpolated line
	vector<glm::vec3> m_centrelineUpVectors;

	vector<glm::vec3> m_leftOffsetPoints;
	vector<glm::vec3> m_rightOffsetPoints;

	unsigned int m_vertexCount;
};