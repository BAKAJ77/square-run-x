#include "OrthogonalCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera() {}

OrthoCamera::OrthoCamera(const glm::vec2& Pos, const glm::vec2& ViewSize) :
	Position(Pos), ViewSize(ViewSize)
{}

OrthoCamera::~OrthoCamera() {}

void OrthoCamera::SetPosition(const glm::vec2& Position)
{
	this->Position = Position;
}

glm::mat4 OrthoCamera::GetViewMatrix() const
{
	return glm::lookAt(glm::vec3(this->Position, 0.1f), glm::vec3(this->Position, 0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 OrthoCamera::GetProjectionMatrix() const
{
	return glm::ortho(0.0f, this->ViewSize.x, 0.0f, this->ViewSize.y);
}

const glm::vec2& OrthoCamera::GetPosition() const { return this->Position; }

const glm::vec2& OrthoCamera::GetViewSize() const { return this->ViewSize; }