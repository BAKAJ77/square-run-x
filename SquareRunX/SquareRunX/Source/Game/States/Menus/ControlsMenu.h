#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/ButtonObject.h"

class ControlsMenu : public GameStateBase
{
private:
	// Scene related things
	const Font* ArialRoundedFont = nullptr, *SansSerifShadedFont = nullptr;
	const Texture* KeybindsTex = nullptr, *TransitionTex1 = nullptr, *TransitionTex2 = nullptr;
	Button BackButton;

	// Backend transition things
	Rect TransitionDest1, TransitionDest2;
	float OpacityMultiplier = 0.0f;
	bool TransitionComplete = false;
public:
	bool PreStageComplete = false, StageOneComplete = false, EndOfState = false; // Needed by 'back' button execute function
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static ControlsMenu* GetGameState();
};