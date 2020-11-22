#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/LevelLoader.h"

#include <vector>
#include <array>

struct LoadedFont
{
	std::string FontName;
	const Font* Data = nullptr;
};

struct LoadedTexture
{
	std::string TextureName;
	const Texture* Data = nullptr;
};

class ResourceLoading : public GameStateBase
{
private:
	std::array<LoadedFont, 3> GameFonts;
	std::array<LoadedTexture, 17> GameTextures;
	mutable std::vector<LevelMap> GameLevels;

	Rect LoadingWheelDest;

	double LoadingWheelRotation = 0.0;
	bool LoadingWheelLoaded = false, FinishedLoading = false, PlayedMenuMusic = false;

	PlayableAudio ThemeAudio;
private:
	void LoadGameFonts();
	void LoadGameTextures();
	void LoadGameLevelMaps();
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static ResourceLoading* GetGameState();

	const Font* GetFont(const std::string& FontName) const;
	const Texture* GetTexture(const std::string& TextureName) const;
	LevelMap* GetLevelMap(int LevelIndex, int ActIndex) const;
};