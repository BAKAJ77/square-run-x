#pragma once
#include "Engine/Core/GameStateBase.h"
#include "Game/Utils/ButtonObject.h"

class NewGameMenu : public GameStateBase
{
private:
	// Scene related things
	const Font* ArialRoundedFont = nullptr, *SansSerifShadedFont = nullptr;
	const Texture* TextBoxTex = nullptr;

	Button ConfirmButton, CancelButton;
	bool NameConfirmed = false;

	double AudioVolume = 1.0;
private:
	void PopLastCharacter(const double& DeltaTime) const;
public:
	void InitState() override;
	void DestroyState() override;

	void UpdateTick(const double& DeltaTime) override;
	void RenderFrame() const override;
public:
	static NewGameMenu* GetGameState();
};