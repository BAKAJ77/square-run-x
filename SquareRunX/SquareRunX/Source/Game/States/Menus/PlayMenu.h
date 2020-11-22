#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/ButtonObject.h"

class PlayMenu : public GameStateBase
{
private:
	// Scene related things
	const Font* ArialRoundedFont = nullptr;
	const Texture* ButtonBoxTex = nullptr, *TransitionTex1 = nullptr, *TransitionTex2 = nullptr;
	Button NewGameButton, LoadGameButton, BackButton;

	// Backend transition things
	Rect TransitionDest1, TransitionDest2;
	float OpacityMultiplier = 0.0f;
	bool TransitionComplete = false, PausedState = false;

	PlayableAudio ThemeAudio;
	double AudioVolume = 1.0;
public:
	bool PreStageComplete = false, StageOneComplete = false, EndOfState = false; // Needed by 'back' button execute function
public:
	void InitState() override;
	void DestroyState() override;

	void PauseState() override;
	void ResumeState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static PlayMenu* GetGameState();
};