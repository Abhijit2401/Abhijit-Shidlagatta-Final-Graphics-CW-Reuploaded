// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include "Common.h"

// A class that provides a wrapper around an OpenGL shader
class CShader
{
public:
	CShader();
	~CShader();
	bool LoadShader(const string& sFile, int iType);
	void DeleteShader();
	bool GetLinesFromFile(const string& sFile, bool bIncludePart, vector<string>* vResult);
	bool IsLoaded() const;
	GLuint GetShaderID() const;

private:
	GLuint m_uiShader; // ID of shader
	int m_iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool m_bLoaded; // Whether shader was loaded and compiled
};

// A class the provides a wrapper around an OpenGL shader program
class CShaderProgram
{
public:
	CShaderProgram();

	void CreateProgram();
	void DeleteProgram();

	bool AddShaderToProgram(CShader* shShader);
	bool LinkProgram();

	void UseProgram();

	GLuint GetProgramID() const;

	void SetUniform(const string& sName, glm::vec2* vVectors, int iCount = 1);
	void SetUniform(const string& sName, const glm::vec2 vVector);
	void SetUniform(const string& sName, glm::vec3* vVectors, int iCount = 1);
	void SetUniform(const string& sName, const glm::vec3 vVector);
	void SetUniform(const string& sName, glm::vec4* vVectors, int iCount = 1);
	void SetUniform(const string& sName, const glm::vec4 vVector);
	void SetUniform(const string& sName, float* fValues, int iCount = 1);
	void SetUniform(const string& sName, const float fValue);
	void SetUniform(const string& sName, glm::mat3* mMatrices, int iCount = 1);
	void SetUniform(const string& sName, const glm::mat3 mMatrix);
	void SetUniform(const string& sName, glm::mat4* mMatrices, int iCount = 1);
	void SetUniform(const string& sName, const glm::mat4 mMatrix);
	void SetUniform(const string& sName, int* iValues, int iCount = 1);
	void SetUniform(const string& sName, const int iValue);

private:
	GLuint m_uiProgram; // ID of program
	bool m_bLinked; // Whether program was linked and is ready to use
};