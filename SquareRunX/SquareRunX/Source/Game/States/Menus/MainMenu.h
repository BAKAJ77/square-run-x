#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/TileObject.h"
#include "Game/Utils/ButtonObject.h"
#include "Game/Utils/ParallaxHandler.h"

#include <vector>

class MainMenu : public GameStateBase
{
private:
	// Scene related things
	const Font* DiscoveryFont = nullptr;
	Button PlayButton, LeaderboardButton, ControlsButton, ExitButton;
	
	ParallaxBKG MenuBackground;

	// Backend things
	//PlayableAudio MainMenuTheme;
	double ButtonOpacity = 0.0, TitleOpacity = 0.0;
public:
	void InitState() override;
	void DestroyState() override;

	void PauseState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static MainMenu* GetGameState();
};