#pragma once
struct GLFWwindow;
typedef unsigned int uint32_t;

enum class CursorState
{
	NORMAL = 0x00034001, // GLFW_CURSOR_NORMAL
	LOCK = 0x00034003 // GLFW_CURSOR_DISABLED
};

class WindowFrame
{
	friend class EngineCore;
private:
	GLFWwindow* LibraryWindow;
	uint32_t Width, Height;
private:
	WindowFrame();
	~WindowFrame();

	void InitFrame();
	void DestroyFrame();
public:
	void RequestClose() const;
	void SetCursorState(CursorState State) const;
	void UpdateWindow() const;

	bool WasRequestedClose() const;
public:
	static WindowFrame& GetSingleton();
	GLFWwindow* GetLibraryWindow() const;

	const uint32_t& GetWidth() const;
	const uint32_t& GetHeight() const;
	float GetTick() const;
};