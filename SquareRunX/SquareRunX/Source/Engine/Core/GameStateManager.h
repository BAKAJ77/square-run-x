#pragma once
#include <vector>

class GameStateBase;
class OrthoCamera;

class GameStateManager
{
	friend class EngineCore;
	friend class GameStateBase;
private:
	std::vector<GameStateBase*> StateStack;
private:
	GameStateManager();
	~GameStateManager();

	void DestroyManager();
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