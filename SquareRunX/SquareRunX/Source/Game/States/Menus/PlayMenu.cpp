#include "PlayMenu.h"
#include "MainMenu.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

void PlayMenu::InitState()
{
	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");

	// Get the textures needed
	this->ButtonBoxTex = ResourceLoading::GetGameState()->GetTexture("Button-Box");
	this->TransitionTex1 = ResourceLoading::GetGameState()->GetTexture("Transition-1");
	this->TransitionTex2 = ResourceLoading::GetGameState()->GetTexture("Transition-2");

	// Setup the buttons needed
	constexpr float BUTTON_SCALE = 2.0f;

	this->NewGameButton = Button({ 0, 0, 445, 100 }, { 295, 600, (int)(445 * BUTTON_SCALE), (int)(100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "NEW GAME",
		[=]() {});

	this->LoadGameButton = Button({ 0, 0, 445, 100 }, { 295, 372, (int)(445 * BUTTON_SCALE), (int)(100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "LOAD GAME",
		[=]() {});

	this->BackButton = Button({ 0, 0, 445, 100 }, 
		{ 400, 90, (int)(445 * (BUTTON_SCALE - 0.5f)), (int)(100 * (BUTTON_SCALE - 0.5f)) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "BACK",
		[=]() { this->EndOfState = true; this->StageOneComplete = false; this->PreStageComplete = false; });

	// Initialize other stuff
	this->TransitionDest1 = { 0, -1400, (int)this->SceneCamera.GetViewSize().x, 1400 };
	this->TransitionDest2 = { (int)this->SceneCamera.GetViewSize().x + 600, 0, 600, (int)this->SceneCamera.GetViewSize().y };
}

void PlayMenu::DestroyState()
{
	this->TransitionComplete = false;
	this->PreStageComplete = false;
	this->StageOneComplete = false;
	this->EndOfState = false;
}

void PlayMenu::UpdateTick(const float& DeltaTime)
{
	if (this->TransitionComplete && !this->EndOfState)
	{
		// Update the buttons
		this->NewGameButton.UpdateButton();
		this->LoadGameButton.UpdateButton();
		this->BackButton.UpdateButton();
	}
	else
	{
		// Handle transition animation when entering/leaving game state
		TransitionHandling::UpdateTransition(this->TransitionComplete, this->PreStageComplete, this->StageOneComplete,
			this->EndOfState, this->TransitionDest1, this->TransitionDest2, this->OpacityMultiplier, this->SceneCamera,
			DeltaTime, [&]() { this->PopState(); });
	}
}

void PlayMenu::RenderFrame() const
{
	// Render transition texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1400 }, this->TransitionDest1, *this->TransitionTex1, 0.0f, true);
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 600, 1080 }, this->TransitionDest2, *this->TransitionTex2, 0.0f, true);

	// Render the button border box texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1000, 530 }, 
		{ 240, (int)(this->SceneCamera.GetViewSize().y / 2.0f) - 235, 1000, 530 }, *this->ButtonBoxTex, 0.0f, true, false, 1.0f, 
		this->OpacityMultiplier);

	// Render the buttons
	this->NewGameButton.RenderButton(this->OpacityMultiplier);
	this->LoadGameButton.RenderButton(this->OpacityMultiplier);
	this->BackButton.RenderButton(this->OpacityMultiplier);
}

PlayMenu* PlayMenu::GetGameState()
{
	static PlayMenu GameState;
	return &GameState;
}
