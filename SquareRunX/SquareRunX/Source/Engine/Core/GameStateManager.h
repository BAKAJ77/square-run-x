#pragma once
#include <vector>
#include <glm/glm.hpp>

enum class TransitionType
{
	REVEAL,
	HIDE
};

namespace Transition
{
	static const glm::vec3 SCREEN_COLOR = glm::vec3(0.0f);
	static constexpr float OPACITY_CHANGE_RATE = 0.01f;
	static constexpr float TIME_STEP = 0.01f;
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
	void HandleStateTransitioning();
private:
	void SwitchState(GameStateBase* State);
	void PushState(GameStateBase* State);
	void PopState();

	void UpdateState(const float& DeltaTime);
	void RenderStates() const;
public:
	static GameStateManager& GetSingleton();
	
	const OrthoCamera* GetCurrentStateCamera() const;
	bool IsStateStackEmpty() const;
};