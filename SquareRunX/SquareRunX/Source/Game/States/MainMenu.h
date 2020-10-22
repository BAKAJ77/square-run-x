#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/TileObject.h"
#include <vector>

class MainMenu : public GameStateBase
{
private:
	// Scene related things
	const Texture* BackgroundTex = nullptr, *TileSheetTex = nullptr;
	std::vector<Tile> VisibleTiles;

	// Backend game state things
	bool StartupComplete = false, EndingState = false;
public:
	void InitState() override;
	void DestroyState() override;
	
	void PauseState() override;
	void ResumeState() override;

	void UpdateTick(const float& DeltaTime) override;
	void RenderFrame() const override;
public:
	static MainMenu* GetGameState();
};