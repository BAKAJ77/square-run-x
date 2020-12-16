#include "PlayMenu.h"
#include "MainMenu.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

// Branching game states
#include "NewGameMenu.h"

void PlayMenu::InitState()
{
	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");

	// Get the textures needed
	this->ButtonBoxTex = ResourceLoading::GetGameState()->GetTexture("Button-Box");
	this->TransitionTex1 = ResourceLoading::GetGameState()->GetTexture("Transition-1");
	this->TransitionTex2 = ResourceLoading::GetGameState()->GetTexture("Transition-2");

	// Setup the buttons needed
	constexpr double BUTTON_SCALE = 2.0;

	this->NewGameButton = Button({ 0, 0, 445, 100 }, { 315.0, 600.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "NEW GAME",
		[=]() { this->PushState(NewGameMenu::GetGameState()); }, 100);

	this->LoadGameButton = Button({ 0, 0, 445, 100 }, { 315.0, 372.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "LOAD GAME",
		[=]() {}, 100);

	this->BackButton = Button({ 0, 0, 445, 100 }, 
		{ 500.0, 150.0, (445 * (BUTTON_SCALE - 0.8)), (100 * (BUTTON_SCALE - 0.8f)) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "BACK",
		[=]() { this->EndOfState = true; this->StageOneComplete = false; this->PreStageComplete = false; }, 60);

	// Initialize other stuff
	this->TransitionDest1 = { 0.0, -1400.0, this->SceneCamera.GetViewSize().x, 1400.0 };
	this->TransitionDest2 = { this->SceneCamera.GetViewSize().x + 600, 0.0, 600.0, this->SceneCamera.GetViewSize().y };

	this->ThemeAudio = AudioPlayer::GetSingleton().PlayAudio("GameFiles/Audio/FILE_SELECT.wav", true, true);
	this->AudioVolume = 1.0;
}

void PlayMenu::DestroyState()
{
	this->OpacityMultiplier = 0.0;

	this->TransitionComplete = false;
	this->PreStageComplete = false;
	this->StageOneComplete = false;
	this->EndOfState = false;
	this->PausedState = false;

	AudioPlayer::GetSingleton().StopAllAudio();
	this->ThemeAudio->drop();
}

void PlayMenu::PauseState()
{
	this->PausedState = true;
}

void PlayMenu::ResumeState()
{
	this->PausedState = false;
}

void PlayMenu::UpdateTick(const double& DeltaTime)
{
	if (this->TransitionComplete && !this->EndOfState)
	{
		if (!this->PausedState)
		{
			// Update the buttons
			this->NewGameButton.UpdateButton();
			this->LoadGameButton.UpdateButton();
			this->BackButton.UpdateButton();
		}
	}
	else
	{
		// Handle transition animation when entering/leaving game state
		TransitionHandling::UpdateTransition(this->TransitionComplete, this->PreStageComplete, this->StageOneComplete,
			this->EndOfState, this->TransitionDest1, this->TransitionDest2, this->OpacityMultiplier, this->SceneCamera,
			DeltaTime, [&]() 
			{ 
				if (this->AudioVolume == 0.0)
					this->PopState();
			});

		if (this->EndOfState)
		{
			Effects::PlayFadeEffect(TransitionType::HIDE, this->AudioVolume, 0.005, DeltaTime);
			this->ThemeAudio->setVolume((float)this->AudioVolume);
		}
	}
}

void PlayMenu::RenderFrame() const
{
	// Render transition texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1400 }, this->TransitionDest1, *this->TransitionTex1, 0.0f, true);
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 600, 1080 }, this->TransitionDest2, *this->TransitionTex2, 0.0f, true);

	if (!this->PausedState)
	{
		// Render the button border box texture
		GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1000, 530 },
			{ 260.0, (this->SceneCamera.GetViewSize().y / 2.0) - 235, 1000.0, 530.0 }, *this->ButtonBoxTex, 0.0f, true, false, 
			1.0f, this->OpacityMultiplier);

		// Render the buttons
		this->NewGameButton.RenderButton(this->OpacityMultiplier, 15);
		this->LoadGameButton.RenderButton(this->OpacityMultiplier, 15);
		this->BackButton.RenderButton(this->OpacityMultiplier);
	}
}

PlayMenu* PlayMenu::GetGameState()
{
	static PlayMenu GameState;
	return &GameState;
}
