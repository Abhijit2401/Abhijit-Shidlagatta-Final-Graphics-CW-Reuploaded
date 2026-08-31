/** Copyright (C) 2011 by Jason L. McKesson **/
/** This file is licensed by the MIT License. **/
#pragma once

#include <stack>
#include <vector>
#include "include\glm\glm.hpp"
#include "include\glm\gtc\type_ptr.hpp"

namespace glutil
{
	// Stack math helper. allows movement, rotation and scaling before passing items throughto the shader
	class MatrixStack
	{
	public:
		MatrixStack() : m_currMatrix(1) {}
		explicit MatrixStack(const glm::mat4& initialMatrix) : m_currMatrix(initialMatrix) {}

		// Stack controls
		void Push() { m_stack.push(m_currMatrix); }
		void Pop() {
			m_currMatrix = m_stack.top();
			m_stack.pop();
		}
		void Reset() { m_currMatrix = m_stack.top(); }
		const glm::mat4& Top() const { return m_currMatrix; }

		// Rotation (pass vector by const ref so it isnt copied into memory)
		void Rotate(const glm::vec3& axis, float angDegCCW);
		void RotateRadians(const glm::vec3& axis, float angRadCCW);
		void RotateX(float angDegCCW);
		void RotateY(float angDegCCW);
		void RotateZ(float angDegCCW);

		// Scale
		void Scale(const glm::vec3& scaleVec);
		void Scale(float scaleX, float scaleY, float scaleZ) { Scale(glm::vec3(scaleX, scaleY, scaleZ)); }
		void Scale(float uniformScale) { Scale(glm::vec3(uniformScale)); }

		// Translation
		void Translate(const glm::vec3& offsetVec);
		void Translate(float transX, float transY, float transZ) { Translate(glm::vec3(transX, transY, transZ)); }

		// Camera projection math
		void LookAt(const glm::vec3& cameraPos, const glm::vec3& lookatPos, const glm::vec3& upDir);
		void Perspective(float degFOV, float aspectRatio, float zNear, float zFar);
		void Orthographic(float left, float right, float bottom, float top, float zNear = -1.0f, float zFar = 1.0f);
		void PixelPerfectOrtho(glm::ivec2 size, glm::vec2 depthRange, bool isTopLeft = true);

		// Matrix application
		void ApplyMatrix(const glm::mat4& theMatrix);
		MatrixStack& operator*=(const glm::mat4& theMatrix) { ApplyMatrix(theMatrix); return *this; }
		void SetMatrix(const glm::mat4& theMatrix);
		void SetIdentity();

	private:
		std::stack<glm::mat4, std::vector<glm::mat4>> m_stack;
		glm::mat4 m_currMatrix;
	};

	// Automatic cleanup wrapper to ensure matrix isnt left on stack
	class PushStack
	{
	public:
		PushStack(MatrixStack& stack) : m_stack(stack) { m_stack.Push(); }
		~PushStack() { m_stack.Pop(); }
		void ResetStack() { m_stack.Reset(); }

	private:
		MatrixStack& m_stack;
		PushStack(const PushStack&);
		PushStack& operator=(const PushStack&);
	};
}