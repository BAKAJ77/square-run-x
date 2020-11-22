#include "GameStateManager.h"
#include "GameStateBase.h"
#include "Engine/Graphics/OrthogonalCamera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Transition
{
	void PlayTransitionScreen(TransitionType Type, const double& DeltaTime)
	{
		if (Type == TransitionType::REVEAL)
		{
			PostProcessing::GetSingleton().SetOpacityLevel(PostProcessing::GetSingleton().GetOpacity() +
				(Transition::OPACITY_CHANGE_RATE * DeltaTime));
		}
		else if (Type == TransitionType::HIDE)
		{
			PostProcessing::GetSingleton().SetOpacityLevel(PostProcessing::GetSingleton().GetOpacity() -
				(Transition::OPACITY_CHANGE_RATE * DeltaTime));
		}
	}
}

namespace Effects
{
	void PlayFadeEffect(TransitionType Type, double& ValueRef, const double& ChangeRate, const double& DeltaTime)
	{
		if (Type == TransitionType::REVEAL)
			ValueRef = std::min(ValueRef + (ChangeRate * DeltaTime), 1.0);
		else if (Type == TransitionType::HIDE)
			ValueRef = std::max(ValueRef - (ChangeRate * DeltaTime), 0.0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GameStateManager::GameStateManager() :
	StartedState(true), EndedState(true), SwitchedState(nullptr), FirstStateAdded(true)
{}

GameStateManager::~GameStateManager() {}

void GameStateManager::DestroyManager() 
{
	for (auto& State : this->StateStack)
		State->DestroyState();
}

void GameStateManager::SwitchState(GameStateBase* State)
{
	if (!this->StateStack.empty())
	{
		this->EndedState = false;
		this->FirstStateAdded = false;
	}

	if(this->SwitchedState != State)
		this->SwitchedState = State;
	
	this->StartedState = false;
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
	{
		this->StateStack.back()->DestroyState();
		this->StateStack.pop_back();
	}

	if (!this->StateStack.empty())
		this->StateStack.back()->ResumeState();
}

void GameStateManager::HandleStateTransitioning(const double& DeltaTime)
{
	// Handle transitioning between states (Note that this only occurs when SwitchState() is called)
	if (!this->StartedState && this->EndedState)
	{
		if (PostProcessing::GetSingleton().GetOpacity() == 1.0f && !this->FirstStateAdded)
		{
			this->StartedState = true;
			this->FirstStateAdded = false;
		}
		else
		{
			// Init and push the next state to be switched to
			if (this->SwitchedState)
			{
				this->SwitchedState->InitState();
				this->StateStack.emplace_back(this->SwitchedState);

				this->SwitchedState = nullptr;
			}

			Transition::PlayTransitionScreen(TransitionType::REVEAL, DeltaTime);
		}
	}
	else if (!this->EndedState)
	{
		if (PostProcessing::GetSingleton().GetOpacity() == 0.0f)
		{
			// Clear the state stack of all existing game states
			for (auto& ExistingState : this->StateStack)
				ExistingState->DestroyState();

			this->StateStack.clear();
			this->EndedState = true;
		}
		else
			Transition::PlayTransitionScreen(TransitionType::HIDE, DeltaTime);
	}
}

void GameStateManager::UpdateState(const double& DeltaTime)
{
	this->HandleStateTransitioning(DeltaTime);

	// Update states in the stack
	uint32_t UpdateCount = 0;
	for (const auto& State : this->StateStack)
	{
		if (UpdateCount == this->StateStack.size() - 1)
			State->UpdateTick(DeltaTime);
		else
		{
			if (State->UpdateAfterPause)
				State->UpdateTick(DeltaTime);
		}

		UpdateCount++;
	}
}

void GameStateManager::RenderStates(const PerformanceCounter* PerfCounter) const
{
	uint32_t RenderCount = 0;
	bool ClearedScreen = false;

	for (const auto& State : this->StateStack)
	{
		if (RenderCount == this->StateStack.size() - 1)
		{
			State->RenderFrame();
			if(PerfCounter)
				PerfCounter->RenderCounter();
		}
		else
		{
			if (State->RenderAfterPause)
				State->RenderFrame();
		}

		if (!ClearedScreen)
		{
			GraphicsRenderer::GetSingleton().RefreshScreen(PostProcessing::GetSingleton().GetFBO());
			ClearedScreen = true;
		}

		GraphicsRenderer::GetSingleton().FlushStack(State->GetStateCamera(), PostProcessing::GetSingleton().GetFBO(), 
			PostProcessing::GetSingleton().GetResolution().x, PostProcessing::GetSingleton().GetResolution().y);

		RenderCount++;
	}
}

GameStateManager& GameStateManager::GetSingleton()
{
	static GameStateManager Singleton;
	return Singleton;
}

bool GameStateManager::IsStateStackEmpty() const
{
	return this->StateStack.empty() && !this->SwitchedState;
}

bool GameStateManager::HasCurrentStateStarted() const
{
	return this->StartedState;
}

const OrthoCamera* GameStateManager::GetCurrentStateCamera() const
{
	if (!this->StateStack.empty())
		return &this->StateStack.back()->GetStateCamera();

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
