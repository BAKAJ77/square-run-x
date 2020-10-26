#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/TileObject.h"
#include "Game/Utils/ButtonObject.h"

#include <vector>

class MainMenu : public GameStateBase
{
private:
	// Scene related things
	const Texture* BackgroundTex = nullptr, *TileSheetTex = nullptr, *MenuTitleTex = nullptr;
	Button PlayButton, LeaderboardButton, ControlsButton, ExitButton;
	
	std::vector<Tile> VisibleTiles;
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const float& DeltaTime) override;
	void RenderFrame() const override;
public:
	static MainMenu* GetGameState();
};