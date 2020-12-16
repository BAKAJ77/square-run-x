#pragma once
#include "Engine/Core/GameStateBase.h"

class GameOverState : public GameStateBase
{
private:
	// Scene related things
	const Font* ArialRoundedFont = nullptr, *DiscoveryFont = nullptr, *FFFFont = nullptr;
	const Texture* GameOverFadeTex = nullptr;

	double GameOverTextOpacity = 0.0, ActionTextOpacity = 0.0;
	bool StateCurrent = true, QuitGame = false;

	// Audio related things
	PlayableAudio GameOverAudio;
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static GameOverState* GetGameState();
};