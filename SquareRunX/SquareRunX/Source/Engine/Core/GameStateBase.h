#pragma once
#include "GameStateManager.h"

// Essential engine modules for the game states
#include "Engine/Core/WindowFrame.h"
#include "Engine/Core/InputHandler.h"
#include "Engine/Debug/LoggingManager.h"
#include "Engine/Graphics/GraphicsRenderer.h"
#include "Engine/Graphics/FontLoaderTTF.h"
#include "Engine/Graphics/ShaderManager.h"
#include "Engine/Graphics/OrthogonalCamera.h"
#include "Engine/Graphics/PostProcessingState.h"
#include "Engine/Graphics/TextureManager.h"

namespace Scene
{
	static constexpr int WIDTH = 1920;
	static constexpr int HEIGHT = 1080;
}

namespace Transition
{
	static const glm::vec3 SCREEN_COLOR = glm::vec3(0.0f);
	static constexpr float OPACITY_CHANGE_RATE = 0.01f;
	static constexpr float TIME_STEP = 0.01f;
}

enum class TransitionType
{
	REVEAL,
	HIDE
};

class GameStateBase
{
	friend class GameStateManager;
protected:
	OrthoCamera SceneCamera;
	bool RenderAfterPause;
protected:
	GameStateBase() :
		RenderAfterPause(true)
	{
		this->SceneCamera = OrthoCamera(glm::vec2(0.0f), glm::vec2(Scene::WIDTH, Scene::HEIGHT));
	}

	virtual ~GameStateBase() {}
public:
	virtual void InitState() = 0;
	virtual void DestroyState() = 0;

	virtual void PauseState() {}
	virtual void ResumeState() {}

	virtual void UpdateTick(const float& DeltaTime) = 0;
	virtual void RenderFrame() const = 0;

	void HandleTransitions(TransitionType Type)
	{
		static float PrevTime = 0.0f;

		if (Type == TransitionType::REVEAL)
		{
			if (WindowFrame::GetSingleton().GetTick() - PrevTime > Transition::TIME_STEP)
			{
				PostProcessing::GetSingleton().SetOpacityLevel(PostProcessing::GetSingleton().GetOpacity() +
					Transition::OPACITY_CHANGE_RATE);

				PrevTime = WindowFrame::GetSingleton().GetTick();
			}
		}
		else if (Type == TransitionType::HIDE)
		{
			if (WindowFrame::GetSingleton().GetTick() - PrevTime > Transition::TIME_STEP)
			{
				PostProcessing::GetSingleton().SetOpacityLevel(PostProcessing::GetSingleton().GetOpacity() -
					Transition::OPACITY_CHANGE_RATE);

				PrevTime = WindowFrame::GetSingleton().GetTick();
			}
		}
	}

	void SwitchState(GameStateBase* State) { GameStateManager::GetSingleton().SwitchState(State); }
	void PushState(GameStateBase* State) { GameStateManager::GetSingleton().PushState(State); }
	void PopState() { GameStateManager::GetSingleton().PopState(); }

	const OrthoCamera& GetStateCamera() const { return this->SceneCamera; }
	bool ShouldRenderAfterPause() const { return this->RenderAfterPause; }
};