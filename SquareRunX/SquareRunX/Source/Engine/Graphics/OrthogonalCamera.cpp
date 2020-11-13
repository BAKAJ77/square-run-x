#include "OrthogonalCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera() {}

OrthoCamera::OrthoCamera(const glm::dvec2& Pos, const glm::dvec2& ViewSize) :
	Position(Pos), ViewSize(ViewSize)
{}

OrthoCamera::~OrthoCamera() {}

void OrthoCamera::SetPosition(const glm::dvec2& Position)
{
	this->Position = Position;
}

glm::mat4 OrthoCamera::GetViewMatrix() const
{
	return glm::lookAt(glm::dvec3(this->Position, 0.1), glm::dvec3(this->Position, 0.0), 
		glm::dvec3(0.0, 1.0, 0.0));
}

glm::mat4 OrthoCamera::GetProjectionMatrix() const
{
	return glm::ortho(0.0, this->ViewSize.x, 0.0, this->ViewSize.y, 0.0, 100.0);
}

const glm::dvec2& OrthoCamera::GetPosition() const { return this->Position; }

const glm::dvec2& OrthoCamera::GetViewSize() const { return this->ViewSize; }