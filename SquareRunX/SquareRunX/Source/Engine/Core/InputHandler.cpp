#include "InputHandler.h"
#include "WindowFrame.h"
#include "Engine/Graphics/PostProcessingState.h"

#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Input
{
	// Input related variables
	std::unordered_map<InputCode, KeyState>* KeyStatesPtr;
	glm::vec2 CursorPosition; 

	// Callback functions
	void CursorPositionCallback(GLFWwindow* Window, double PosX, double PosY)
	{
		// Map the cursor position within the range of the ortho camera
		CursorPosition.x = (1920.0f / static_cast<float>(WindowFrame::GetSingleton().GetWidth())) * static_cast<float>(PosX);
		CursorPosition.y = (1080.0f / static_cast<float>(WindowFrame::GetSingleton().GetHeight())) *
			(static_cast<float>(WindowFrame::GetSingleton().GetHeight()) - static_cast<float>(PosY));
	}

	void KeyStateCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
	{
		const auto& KeyObj = KeyStatesPtr->find((InputCode)Key);

		switch (Action)
		{
		case GLFW_PRESS:
		{
			if (KeyObj == KeyStatesPtr->end())
				KeyStatesPtr->insert(std::pair<InputCode, KeyState>(static_cast<InputCode>(Key), KeyState::PRESSED));
			else
				KeyStatesPtr->at((InputCode)Key) = KeyState::PRESSED;
		}
			break;
		case GLFW_REPEAT:
		{
			if (KeyObj == KeyStatesPtr->end())
				KeyStatesPtr->insert(std::pair<InputCode, KeyState>(static_cast<InputCode>(Key), KeyState::REPEAT));
			else
				KeyStatesPtr->at((InputCode)Key) = KeyState::REPEAT;
		}
			break;
		case GLFW_RELEASE:
		{
			if (KeyObj == KeyStatesPtr->end())
				KeyStatesPtr->insert(std::pair<InputCode, KeyState>(static_cast<InputCode>(Key), KeyState::RELEASED));
			else
				KeyStatesPtr->at((InputCode)Key) = KeyState::RELEASED;
		}
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InputHandler::InputHandler() {}

InputHandler::~InputHandler() {}

void InputHandler::InitHandler()
{
	Input::KeyStatesPtr = &this->KeyStates;

	glfwSetKeyCallback(WindowFrame::GetSingleton().GetLibraryWindow(), Input::KeyStateCallback);
	glfwSetCursorPosCallback(WindowFrame::GetSingleton().GetLibraryWindow(), Input::CursorPositionCallback);
}

bool InputHandler::WasKeyPressed(InputCode Key) const
{
	const auto& KeyObj = this->KeyStates.find(Key);
	
	if (KeyObj != this->KeyStates.end())
		return KeyObj->second == KeyState::PRESSED;
	else
		return false;
}

bool InputHandler::WasKeyReleased(InputCode Key) const
{
	const auto& KeyObj = this->KeyStates.find(Key);

	if (KeyObj != this->KeyStates.end())
		return KeyObj->second == KeyState::RELEASED;
	else
		return false;
}

bool InputHandler::IsKeyHeld(InputCode Key) const
{
	return glfwGetKey(WindowFrame::GetSingleton().GetLibraryWindow(), static_cast<int>(Key)) == GLFW_PRESS;
}

bool InputHandler::WasMouseButtonClicked(InputCode Button) const
{
	return glfwGetMouseButton(WindowFrame::GetSingleton().GetLibraryWindow(), static_cast<int>(Button)) == GLFW_PRESS;
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