// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include "Common.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"

// Handles the 3D viewport and math matrices for the shaders
class CCamera {
public:
	CCamera();
	~CCamera();

	glm::vec3 GetPosition() const;
	glm::vec3 GetView() const;
	glm::vec3 GetUpVector() const;
	glm::vec3 GetStrafeVector() const;

	// Pointers so the shaders can grab them easily
	glm::mat4* GetPerspectiveProjectionMatrix();
	glm::mat4* GetOrthographicProjectionMatrix();
	glm::mat4 GetViewMatrix() const;

	void SetPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane);
	void SetOrthographicProjectionMatrix(int width, int height);

	// Nmormal matrix fixes lighting when objects get rotated/scaled
	glm::mat3 ComputeNormalMatrix(const glm::mat4& modelViewMatrix) const;

	// Camera movement logic
	void Set(const glm::vec3& position, const glm::vec3& viewpoint, const glm::vec3& upVector);
	void RotateViewPoint(float angle, const glm::vec3& axis);
	void SetViewByMouse();
	void TranslateByKeyboard(double dt);
	void Strafe(double direction);
	void Advance(double direction);
	void Update(double dt);

private:
	glm::vec3 m_position;
	glm::vec3 m_view;
	glm::vec3 m_upVector;
	glm::vec3 m_strafeVector;

	float m_speed;

	glm::mat4 m_perspectiveProjectionMatrix;
	glm::mat4 m_orthographicProjectionMatrix;
};