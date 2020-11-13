#include "WindowFrame.h"
#include "Engine/Filesystem/CFGConfigLoader.h"
#include "Engine/Debug/LoggingManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace
{
	// These pointers are for the callback function so it can access the window size variables
	uint32_t* WidthPtr = nullptr, *HeightPtr = nullptr;
}

void WindowResizeCallback(GLFWwindow* Window, int Width, int Height)
{
	*WidthPtr = static_cast<uint32_t>(Width);
	*HeightPtr = static_cast<uint32_t>(Height);
}

WindowFrame::WindowFrame() :
	Width(0), Height(0), LibraryWindow(nullptr)
{}

WindowFrame::~WindowFrame() {}

void WindowFrame::InitFrame()
{
	// Set the pointers to point to the window size variables
	WidthPtr = &this->Width;
	HeightPtr = &this->Height;

	// Retrieve the window properties from the config file
	CFGConfigLoader::GetSingleton().OpenFile("WindowOptions");
	
	this->Width = CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("Width"));
	this->Height = CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("Height"));
	const bool FULLSCREEN = CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("Fullscreen"));
	const bool RESIZABLE = CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("Resizable"));
	const bool VSYNC_ENABLED = CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("VSyncEnabled"));
	
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
	glfwSetWindowSizeCallback(this->LibraryWindow, WindowResizeCallback);

	if(VSYNC_ENABLED)
		glfwSwapInterval(1);
	
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

	this->WindowStartTime = std::chrono::high_resolution_clock::now(); // Get current (start) time of window execution
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

double WindowFrame::GetTick() const 
{ 
	const auto CURRENT_TIME_TICK = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
		CURRENT_TIME_TICK - this->WindowStartTime).count();
}