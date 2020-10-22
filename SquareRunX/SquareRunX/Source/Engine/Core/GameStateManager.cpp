#include "GameStateManager.h"
#include "GameStateBase.h"

#include "Engine/Graphics/OrthogonalCamera.h"

GameStateManager::GameStateManager() {}

GameStateManager::~GameStateManager() {}

void GameStateManager::DestroyManager() 
{
	for (auto& State : this->StateStack)
		State->DestroyState();
}

void GameStateManager::SwitchState(GameStateBase* State)
{
	if (!this->StateStack.empty())
		this->StateStack.pop_back();

	State->InitState();
	this->StateStack.emplace_back(State);
}

void GameStateManager::PushState(GameStateBase* State)
{
	if (!this->StateStack.empty())
		this->StateStack.back()->PauseState();

	State->InitState();
	this->StateStack.emplace_back(State);
}

void GameStateManager::PopState()
{
	if (!this->StateStack.empty())
		this->StateStack.pop_back();

	if (!this->StateStack.empty())
		this->StateStack.back()->ResumeState();
}

void GameStateManager::UpdateState(const float& DeltaTime)
{
	if (!this->StateStack.empty())
		this->StateStack.back()->UpdateTick(DeltaTime);
}

void GameStateManager::RenderStates() const
{
	bool CurrentState = true;
	for (const auto& State : this->StateStack)
	{
		if (CurrentState)
		{
			State->RenderFrame();
			CurrentState = false;
		}
		else
		{
			if (State->ShouldRenderAfterPause())
				State->RenderFrame();
		}
	}
}

GameStateManager& GameStateManager::GetSingleton()
{
	static GameStateManager Singleton;
	return Singleton;
}

bool GameStateManager::IsStateStackEmpty() const
{
	return this->StateStack.empty();
}

const OrthoCamera* GameStateManager::GetCurrentStateCamera() const
{
	if (!this->StateStack.empty())
		return &this->StateStack.back()->GetStateCamera();

	return nullptr;
}
