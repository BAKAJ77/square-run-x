#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Entities/PlayerEntity.h"
#include "Game/Utils/GameSave.h"

class LevelMap;

class GameplayState : public GameStateBase
{
private:
	// Scene related things
	const Font* ArialRoundedFont = nullptr;
	const Texture* IntroTransitionTex1 = nullptr, *IntroTransitionTex2 = nullptr, *IntroTransitionTex3 = nullptr;

	// Other game things
	PlayerEntity PlayerController;
	LevelMap* CurrentLevel = nullptr;
	GameSave CurrentGameSave;

	// Backend transition things
	Rect IntroTransitionDest1, IntroTransitionDest2, IntroTransitionDest3;
	float IntroTextOpacity = 0.0f;
	bool IntroComplete = false, ShouldRenderState = false, IntroCurrentlyDisplayed = false, ScreenFadedAway = false;

	PlayableAudio ThemeAudio;
	double AudioVolume = 1.0;
private:
	void SetCurrentGameSave(const GameSave& CurrentGameSave);
	void HandleIntroTransition(const double& DeltaTime, bool ResetIntroState = false, double DelayTime = 0.0,
		bool FadeAway = false);
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static GameplayState* GetGameState(const GameSave& CurrentGameSave);
};