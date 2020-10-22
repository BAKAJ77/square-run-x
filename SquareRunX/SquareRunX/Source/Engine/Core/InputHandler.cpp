#include "InputHandler.h"
#include "WindowFrame.h"
#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Input
{
	// Input related variables
	glm::vec2 CursorPosition; 

	// Callback functions
	void CursorPositionCallback(GLFWwindow* Window, double PosX, double PosY)
	{
		CursorPosition.x = static_cast<float>(PosX);
		CursorPosition.y = static_cast<float>(PosY);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InputHandler::InputHandler() {}

InputHandler::~InputHandler() {}

void InputHandler::InitHandler()
{
	glfwSetCursorPosCallback(WindowFrame::GetSingleton().GetLibraryWindow(), Input::CursorPositionCallback);
}

bool InputHandler::WasKeyPressed(InputCode key) const
{
	return glfwGetKey(WindowFrame::GetSingleton().GetLibraryWindow(), static_cast<int>(key)) == GLFW_PRESS;
}

bool InputHandler::WasMouseButtonClicked(InputCode button) const
{
	return glfwGetMouseButton(WindowFrame::GetSingleton().GetLibraryWindow(), static_cast<int>(button)) == GLFW_PRESS;
}

const glm::vec2& InputHandler::GetMousePosition() const
{
	return Input::CursorPosition;
}

InputHandler& InputHandler::GetSingleton()
{
	static InputHandler Singleton;
	return Singleton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////