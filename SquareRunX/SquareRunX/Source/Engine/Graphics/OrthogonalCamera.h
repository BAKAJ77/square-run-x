#pragma once
#include <glm/glm.hpp>

class OrthoCamera
{
private:
	glm::dvec2 Position;
	glm::dvec2 ViewSize;
public:
	OrthoCamera();
	OrthoCamera(const glm::dvec2& Pos, const glm::dvec2& ViewSize);
	~OrthoCamera();

	void SetPosition(const glm::dvec2& Position);
	
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
public:
	const glm::dvec2& GetPosition() const;
	const glm::dvec2& GetViewSize() const;
};