#include "EngineCore.h"

// Singleton Class Includes
#include "Engine/Core/WindowFrame.h"
#include "Engine/Filesystem/FileHandler.h"
#include "Engine/Core/InputHandler.h"
#include "Engine/Graphics/GraphicsRenderer.h"
#include "Engine/Graphics/ShaderManager.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/FontLoaderTTF.h"
#include "Engine/Graphics/PostProcessingState.h"
#include "Engine/Core/GameStateManager.h"
#include "Engine/Debug/LoggingManager.h"
#include "Engine/Filesystem/CFGConfigLoader.h"

// Other Includes
#include "Game/States/Other/ResourceLoading.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
	constexpr double TIME_STEP = 1.0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

EngineCore::EngineCore() :
	DisplayPerformanceCounter(false)
{
	this->InitSingletons();
	this->MainLoop();
}

EngineCore::~EngineCore()
{
	GameStateManager::GetSingleton().DestroyManager();
	FontLoaderTTF::GetSingleton().DestroyLoader();
	ShaderManager::GetSingleton().DestroyManager();
	TextureManager::GetSingleton().DestroyManager();
	WindowFrame::GetSingleton().DestroyFrame();
	FileHandler::GetSingleton().DestroyHandler(); // Should be destroyed last since lots of classes rely on it
}

void EngineCore::InitSingletons()
{
	FileHandler::GetSingleton().InitHandler(); // Should be initialized first since lots of classes rely on it
	LogManager::GetSingleton().InitManager();
	WindowFrame::GetSingleton().InitFrame();
	TextureManager::GetSingleton().InitManager();
	GraphicsRenderer::GetSingleton().InitRenderer();
	ShaderManager::GetSingleton().InitManager();
	FontLoaderTTF::GetSingleton().InitLoader();
	PostProcessing::GetSingleton().InitProcess();
	InputHandler::GetSingleton().InitHandler();

	this->LoadConfigSettings();
	GameStateManager::GetSingleton().SwitchState(ResourceLoading::GetGameState());
}

void EngineCore::LoadConfigSettings()
{
	// Load the neccesary config setting values
	CFGConfigLoader::GetSingleton().OpenFile("Debugging");
	this->DisplayPerformanceCounter = 
		CFGElement::ToDataValue<bool>(*CFGConfigLoader::GetSingleton().QueryElement("DisplayPerfCounter"));
	CFGConfigLoader::GetSingleton().CloseFile();
}

void EngineCore::MainLoop()
{
	// Run the main game loop
	this->PerfCounter.InitializeCounter();
	double TimeAccumulator = 0.0, FrameTime = 0.0f;

	while (!WindowFrame::GetSingleton().WasRequestedClose() && !GameStateManager::GetSingleton().IsStateStackEmpty())
	{
		// Update the game logic
		TimeAccumulator += FrameTime;
		while (TimeAccumulator >= TIME_STEP)
		{
			this->UpdateTick(TIME_STEP);
			TimeAccumulator -= TIME_STEP;
		}

		// Render to screen and time it
		const double LAST_RENDER_TIME = WindowFrame::GetSingleton().GetTick();

		this->RenderTick();
		WindowFrame::GetSingleton().UpdateWindow();

		const double CURRENT_RENDER_TIME = WindowFrame::GetSingleton().GetTick();
		FrameTime = CURRENT_RENDER_TIME - LAST_RENDER_TIME;
	}
}

void EngineCore::UpdateTick(const double& DeltaTime) 
{
	// Update in-use game states and FPS counter
	GameStateManager::GetSingleton().UpdateState(DeltaTime);
	this->PerfCounter.IncrementUpdateCounter();
}

void EngineCore::RenderTick() const 
{
	// Render the state scenes to the post-processing FBO
	if(this->DisplayPerformanceCounter)
		GameStateManager::GetSingleton().RenderStates(&this->PerfCounter);
	else
		GameStateManager::GetSingleton().RenderStates(nullptr);
	
	// Render the processed scene texture to the screen
	PostProcessing::GetSingleton().RenderProcessedFrame();

	// Increment the frames rendered performance counter
	this->PerfCounter.IncrementRenderCounter();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int Argc, char** Argv)
{
	EngineCore Core;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////