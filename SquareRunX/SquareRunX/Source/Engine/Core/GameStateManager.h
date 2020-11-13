#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Engine/Debug/PerformanceCounter.h"

enum class TransitionType
{
	REVEAL,
	HIDE
};

namespace Transition
{
	static const glm::vec3 SCREEN_COLOR = glm::vec3(0.0f);
	static constexpr float OPACITY_CHANGE_RATE = 0.001f;

	extern void PlayTransitionScreen(TransitionType Type, const double& DeltaTime);
}

class GameStateBase;
class OrthoCamera;

class GameStateManager
{
	friend class EngineCore;
	friend class GameStateBase;
private:
	std::vector<GameStateBase*> StateStack;

	GameStateBase* SwitchedState;
	bool StartedState, EndedState, FirstStateAdded;
private:
	GameStateManager();
	~GameStateManager();

	void DestroyManager();
	void HandleStateTransitioning(const double& DeltaTime);
private:
	void SwitchState(GameStateBase* State);
	void PushState(GameStateBase* State);
	void PopState();

	void UpdateState(const double& DeltaTime);
	void RenderStates(const PerformanceCounter* PerfCounter) const;
public:
	static GameStateManager& GetSingleton();
	
	const OrthoCamera* GetCurrentStateCamera() const;
	bool IsStateStackEmpty() const;
	bool HasCurrentStateStarted() const;
};