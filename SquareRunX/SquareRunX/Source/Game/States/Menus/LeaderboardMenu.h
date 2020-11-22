#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/ButtonObject.h"
#include <vector>

struct PlayerData
{
	std::string Name;
	int Score = 0;
};

class LeaderboardMenu : public GameStateBase
{
private:
	std::vector<PlayerData> LeaderboardArray;

	// Scene related things
	const Font* ArialRoundedFont = nullptr, *SansSerifShadedFont = nullptr;
	const Texture* TransitionTex1 = nullptr, *TransitionTex2 = nullptr,
		*LeaderboardSkinTex = nullptr;
	Button BackButton;

	// Backend transition things
	Rect TransitionDest1, TransitionDest2;
	float OpacityMultiplier = 0.0f;
	bool TransitionComplete = false;

	PlayableAudio ThemeAudio;
	double AudioVolume = 1.0;
public:
	bool PreStageComplete = false, StageOneComplete = false, EndOfState = false; // Needed by 'back' button execute function
private:
	void RenderLeaderboardIndex(uint8_t Index) const;
	void ExtractLeaderboardData();
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static LeaderboardMenu* GetGameState();
};