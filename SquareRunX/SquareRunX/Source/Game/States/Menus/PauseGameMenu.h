#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/ButtonObject.h"

class PauseGameMenu : public GameStateBase
{
private:
	// Scene related things
	const Font* FFFFont = nullptr;
	const Texture* ButtonBoxTex = nullptr;

	Button ResumeButton, QuitButton;

	// Backend things
	bool ReturnToGameplay = false, ReturnToMainMenu = false;
	double InterfaceOpacity = 0.0;
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static PauseGameMenu* GetGameState();
};