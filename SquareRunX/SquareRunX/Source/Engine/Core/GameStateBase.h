#pragma once
#include "GameStateManager.h"

// Essential engine modules for the game states
#include "Engine/Core/WindowFrame.h"
#include "Engine/Core/InputHandler.h"
#include "Engine/Core/AudioPlayer.h"
#include "Engine/Debug/LoggingManager.h"
#include "Engine/Graphics/GraphicsRenderer.h"
#include "Engine/Graphics/FontLoaderTTF.h"
#include "Engine/Graphics/ShaderManager.h"
#include "Engine/Graphics/OrthogonalCamera.h"
#include "Engine/Graphics/PostProcessingState.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Filesystem/FileHandler.h"

namespace Scene
{
	static constexpr int WIDTH = 1920;
	static constexpr int HEIGHT = 1080;
}

class GameStateBase
{
	friend class GameStateManager;
protected:
	OrthoCamera SceneCamera;
	bool RenderAfterPause, UpdateAfterPause;
protected:
	GameStateBase() :
		RenderAfterPause(true), UpdateAfterPause(true)
	{
		this->SceneCamera = OrthoCamera(glm::vec2(0.0f), glm::vec2(Scene::WIDTH, Scene::HEIGHT));
	}

	virtual ~GameStateBase() {}
public:
	virtual void InitState() = 0;
	virtual void DestroyState() = 0;

	virtual void PauseState() {}
	virtual void ResumeState() {}

	virtual void UpdateTick(const double& DeltaTime) = 0;
	virtual void RenderFrame() const = 0;

	void SwitchState(GameStateBase* State) { GameStateManager::GetSingleton().SwitchState(State); }
	void PushState(GameStateBase* State) { GameStateManager::GetSingleton().PushState(State); }
	void PopState() { GameStateManager::GetSingleton().PopState(); }

	const OrthoCamera& GetStateCamera() const { return this->SceneCamera; }
};

