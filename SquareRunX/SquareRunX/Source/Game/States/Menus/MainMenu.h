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
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static MainMenu* GetGameState();
};