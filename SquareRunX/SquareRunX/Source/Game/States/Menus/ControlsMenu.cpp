#include "ControlsMenu.h"
#include "MainMenu.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

void ControlsMenu::InitState()
{
	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");
	this->SansSerifShadedFont = ResourceLoading::GetGameState()->GetFont("Sans-Serif-Shaded");

	// Get the textures needed
	this->TransitionTex1 = ResourceLoading::GetGameState()->GetTexture("Transition-1");
	this->TransitionTex2 = ResourceLoading::GetGameState()->GetTexture("Transition-2");
	this->KeybindsTex = ResourceLoading::GetGameState()->GetTexture("Keybinds-Img");
	
	// Setup the buttons needed
	this->BackButton = Button({ 0, 0, 445, 100 }, { 40, 30, 445, 100 },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "BACK", 
		[=]() { this->EndOfState = true; this->StageOneComplete = false; this->PreStageComplete = false; });

	// Initialize other stuff
	this->TransitionDest1 = { 0.0, -1400.0, this->SceneCamera.GetViewSize().x, 1400.0 };
	this->TransitionDest2 = { this->SceneCamera.GetViewSize().x + 600, 0.0, 600.0, this->SceneCamera.GetViewSize().y };
}

void ControlsMenu::DestroyState() 
{
	this->TransitionComplete = false;
	this->PreStageComplete = false;
	this->StageOneComplete = false;
	this->EndOfState = false;
}

void ControlsMenu::UpdateTick(const double& DeltaTime)
{
	if (this->TransitionComplete && !this->EndOfState)
	{
		// Update the buttons
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

void ControlsMenu::RenderFrame() const
{
	// Render transition texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1400 }, this->TransitionDest1, *this->TransitionTex1, 0.0f, true);
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 600, 1080 }, this->TransitionDest2, *this->TransitionTex2, 0.0f, true);

	Rect SourceRect, DestinationRect;

	// Render the controls menu title
	GraphicsRenderer::GetSingleton().RenderText({ 40, 940 }, 100, "CONTROLS", *this->SansSerifShadedFont,
		{ glm::vec3(0.0f), this->OpacityMultiplier });

	// Render the keybinds texture
	SourceRect = { 0, 0, 1300, 680 };
	DestinationRect = { 20.0, 870 - SourceRect.h, SourceRect.w, SourceRect.h };

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->KeybindsTex, 0.0f, true, false, 1.0f,
		this->OpacityMultiplier);

	// Render the buttons
	this->BackButton.RenderButton(this->OpacityMultiplier);
}

ControlsMenu* ControlsMenu::GetGameState()
{
	static ControlsMenu GameState;
	return &GameState;
}
