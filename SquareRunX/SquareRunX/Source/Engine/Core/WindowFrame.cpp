#include "WindowFrame.h"
#include "Engine/Filesystem/CFGConfigLoader.h"
#include "Engine/Debug/LoggingManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

WindowFrame::WindowFrame() :
	Width(0), Height(0), LibraryWindow(nullptr)
{}

WindowFrame::~WindowFrame() {}

void WindowFrame::InitFrame()
{
	// Retrieve the window properties from the config file
	CFGConfigLoader::GetSingleton().OpenFile("WindowOptions");
	this->Width = CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("Width"));
	this->Height = CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("Height"));
	const bool FULLSCREEN = CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("Fullscreen"));
	const bool RESIZABLE = CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("Resizable"));
	CFGConfigLoader::GetSingleton().CloseFile();

	// Initialize the GLFW library before creating window
	if (glfwInit() < 0)
		LogManager::GetSingleton().OutputLog("Failed to initailize GLFW.", LogLevel::FATAL);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, RESIZABLE);

	// Now create the window for the game
	FULLSCREEN ?
		this->LibraryWindow = glfwCreateWindow(this->Width, this->Height, "Square Run X", glfwGetPrimaryMonitor(), nullptr) :
		this->LibraryWindow = glfwCreateWindow(this->Width, this->Height, "Square Run X", nullptr, nullptr);

	if(!this->LibraryWindow)
		LogManager::GetSingleton().OutputLog("Failed to create the window.", LogLevel::FATAL);

	glfwMakeContextCurrent(this->LibraryWindow);
	
	// Set the position of the window to the centre of the monitor
	const GLFWvidmode* VIDEO_MODE = glfwGetVideoMode(glfwGetPrimaryMonitor());
	const int& VIDEO_WIDTH = VIDEO_MODE->width;
	const int& VIDEO_HEIGHT = VIDEO_MODE->height;

	glfwSetWindowPos(this->LibraryWindow, (VIDEO_WIDTH / 2) - (this->Width / 2), (VIDEO_HEIGHT / 2) - (this->Height / 2));

	// Finally load the OpenGL functions using GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		LogManager::GetSingleton().OutputLog("Failed to load OpenGL functions.", LogLevel::FATAL);

	// Output the OpenGL context version (Not necessary)
	LogManager::GetSingleton().OutputLog(reinterpret_cast<const char*>(glGetString(GL_VERSION)), LogLevel::INFO);
}

void WindowFrame::DestroyFrame() { glfwTerminate(); }

void WindowFrame::RequestClose() const { glfwSetWindowShouldClose(this->LibraryWindow, true); }

void WindowFrame::SetCursorState(CursorState State) const
{ 
	glfwSetInputMode(this->LibraryWindow, GLFW_CURSOR, static_cast<int>(State)); 
}

void WindowFrame::UpdateWindow() const
{
	glfwPollEvents();
	glfwSwapBuffers(this->LibraryWindow);
}

bool WindowFrame::WasRequestedClose() const { return glfwWindowShouldClose(this->LibraryWindow); }

WindowFrame& WindowFrame::GetSingleton()
{
	static WindowFrame Singleton;
	return Singleton;
}

GLFWwindow* WindowFrame::GetLibraryWindow() const { return this->LibraryWindow; }

const uint32_t& WindowFrame::GetWidth() const { return this->Width; }

const uint32_t& WindowFrame::GetHeight() const { return this->Height; }

float WindowFrame::GetTick() const { return static_cast<float>(glfwGetTime()); }