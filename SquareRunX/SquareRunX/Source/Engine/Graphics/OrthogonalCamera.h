#pragma once
#include <glm/glm.hpp>

class OrthoCamera
{
private:
	glm::vec2 Position;
	glm::vec2 ViewSize;
public:
	OrthoCamera();
	OrthoCamera(const glm::vec2& Pos, const glm::vec2& ViewSize);
	~OrthoCamera();

	void SetPosition(const glm::vec2& Position);
	
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
public:
	const glm::vec2& GetPosition() const;
	const glm::vec2& GetViewSize() const;
};