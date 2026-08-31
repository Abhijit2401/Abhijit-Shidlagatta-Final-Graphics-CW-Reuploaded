// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// Major logic written/edited by me:
// - 4 Distinct Camera Modes (1st Person, 3rd Person, Top Down, Free-Fly)
// - Gimbal-lock prevention for mouse look
// - Dynamic Camera Shake math based on collision cooldown intensity
// - View Matrix and Normal Matrix extraction for shaders
// ---------------------------------------------------------
#include "Common.h"
#include "Camera.h"
#include "gamewindow.h"
#define _USE_MATH_DEFINES
#include <math.h>

CCamera::CCamera()
{
	m_position = glm::vec3(0.0f, 10.0f, 100.0f);
	m_view = glm::vec3(0.0f, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_speed = 0.025f;
}

CCamera::~CCamera() {}

void CCamera::Set(const glm::vec3& position, const glm::vec3& viewpoint, const glm::vec3& upVector)
{
	m_position = position;
	m_view = viewpoint;
	m_upVector = upVector;
}

void CCamera::RotateViewPoint(float fAngle, const glm::vec3& vPoint)
{
	glm::vec3 vView = m_view - m_position;

	// Uses system M_PI here for exact precision instead of 3.14
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), fAngle * 180.0f / (float)M_PI, vPoint);
	glm::vec4 newView = R * glm::vec4(vView, 1.0f);

	m_view = m_position + glm::vec3(newView);
}

void CCamera::Strafe(double direction)
{
	float speed = (float)(m_speed * direction);

	m_position.x += m_strafeVector.x * speed;
	m_position.z += m_strafeVector.z * speed;

	m_view.x += m_strafeVector.x * speed;
	m_view.z += m_strafeVector.z * speed;
}

void CCamera::Advance(double direction)
{
	float speed = (float)(m_speed * direction);

	glm::vec3 view = glm::normalize(m_view - m_position);
	m_position += view * speed;
	m_view += view * speed;
}

void CCamera::SetViewByMouse()
{
	int middle_x = GameWindow::SCREEN_WIDTH >> 1;
	int middle_y = GameWindow::SCREEN_HEIGHT >> 1;

	static float rotation_x = 0.0f;

	POINT mouse;
	GetCursorPos(&mouse);

	if (mouse.x == middle_x && mouse.y == middle_y) return;

	SetCursorPos(middle_x, middle_y);

	float angle_y = (float)(middle_x - mouse.x) / 10000.0f;
	float angle_z = (float)(middle_y - mouse.y) / 10000.0f;

	rotation_x -= angle_z;

	// Clamps vertical rotation so the camera doesnt go upside down
	float maxAngle = 1.56f;

	if (rotation_x > maxAngle) {
		rotation_x = maxAngle;
	}
	else if (rotation_x < -maxAngle) {
		rotation_x = -maxAngle;
	}
	else {
		glm::vec3 cross = glm::cross(m_view - m_position, m_upVector);
		glm::vec3 axis = glm::normalize(cross);
		RotateViewPoint(angle_z, axis);
	}

	RotateViewPoint(angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
}

// For freecam mode
void CCamera::TranslateByKeyboard(double dt)
{
	if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) Advance(1.0 * dt);
	if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) Advance(-1.0 * dt);
	if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) Strafe(-1.0 * dt);
	if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) Strafe(1.0 * dt);
}

void CCamera::Update(double dt)
{
	// Recalculates the strafe vector based on where we are currently facing
	glm::vec3 vector = glm::cross(m_view - m_position, m_upVector);
	m_strafeVector = glm::normalize(vector);

	SetViewByMouse();
	TranslateByKeyboard(dt);
}

void CCamera::SetPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	m_perspectiveProjectionMatrix = glm::perspective(fov, aspectRatio, nearClippingPlane, farClippingPlane);
}

void CCamera::SetOrthographicProjectionMatrix(int width, int height)
{
	m_orthographicProjectionMatrix = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

glm::mat4 CCamera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_view, m_upVector);
}

glm::mat3 CCamera::ComputeNormalMatrix(const glm::mat4& modelViewMatrix) const
{
	// Converts 4x4 transform to a 3x3 normal matrix to fix shader lighting
	return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
}

glm::vec3 CCamera::GetPosition() const { return m_position; }
glm::vec3 CCamera::GetView() const { return m_view; }
glm::vec3 CCamera::GetUpVector() const { return m_upVector; }
glm::vec3 CCamera::GetStrafeVector() const { return m_strafeVector; }
glm::mat4* CCamera::GetPerspectiveProjectionMatrix() { return &m_perspectiveProjectionMatrix; }
glm::mat4* CCamera::GetOrthographicProjectionMatrix() { return &m_orthographicProjectionMatrix; }