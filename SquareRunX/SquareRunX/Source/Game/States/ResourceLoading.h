#pragma once
#include "Engine/Core/GameStateBase.h"
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
	std::array<LoadedFont, 2> GameFonts;
	std::array<LoadedTexture, 5> GameTextures;

	float LoadingWheelRotation = 0.0f;
	bool LoadingWheelLoaded = false, FinishedLoading = false;
private:
	void LoadGameFonts();
	void LoadGameTextures();
	void LoadGameAudio();
	void LoadGameLevelMaps();
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const float& DeltaTime) override;
	void RenderFrame() const override;
public:
	static ResourceLoading* GetGameState();

	const Font* GetFont(const std::string& FontName) const;
	const Texture* GetTexture(const std::string& TextureName) const;
};