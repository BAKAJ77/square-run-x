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

// Other Includes
#include "Game/States/Other/ResourceLoading.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////

EngineCore::EngineCore()
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
	WindowFrame::GetSingleton().InitFrame();
	TextureManager::GetSingleton().InitManager();
	GraphicsRenderer::GetSingleton().InitRenderer();
	ShaderManager::GetSingleton().InitManager();
	FontLoaderTTF::GetSingleton().InitLoader();
	PostProcessing::GetSingleton().InitProcess();
	InputHandler::GetSingleton().InitHandler();

	GameStateManager::GetSingleton().SwitchState(ResourceLoading::GetGameState());
}

void EngineCore::MainLoop()
{
	float PreviousTime = 0.0f;
	while (!WindowFrame::GetSingleton().WasRequestedClose() &&
		!GameStateManager::GetSingleton().IsStateStackEmpty())
	{
		// Calculate the elapsed time of the previous game tick
		const float CURRENT_TIME = WindowFrame::GetSingleton().GetTick();
		const float DELTA_TIME = CURRENT_TIME - PreviousTime;
		PreviousTime = CURRENT_TIME;

		this->UpdateTick(DELTA_TIME);
		this->RenderTick();

		WindowFrame::GetSingleton().UpdateWindow();
	}
}

void EngineCore::UpdateTick(const float& DeltaTime) 
{
	GameStateManager::GetSingleton().UpdateState(DeltaTime);
}

void EngineCore::RenderTick() const 
{
	// Render the state scenes to the post-processing FBO
	GameStateManager::GetSingleton().RenderStates();
	
	// Render the processed scene texture to the screen
	PostProcessing::GetSingleton().RenderProcessedFrame();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int Argc, char** Argv)
{
	EngineCore Core;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////