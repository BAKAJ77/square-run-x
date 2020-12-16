#include "PauseGameMenu.h"
#include "Game/States/Other/ResourceLoading.h"

#include <thread>
#include <chrono>

// Branching game states
#include "Game/States/Menus/MainMenu.h"

using namespace std::chrono_literals;

void PauseGameMenu::InitState()
{
	WindowFrame::GetSingleton().SetCursorState(CursorState::NORMAL);

	// Get the fonts needed
	this->FFFFont = ResourceLoading::GetGameState()->GetFont("FFF-Forwa");

	// Get the textures needed
	this->ButtonBoxTex = ResourceLoading::GetGameState()->GetTexture("Button-Box");

	// Setup the buttons
	constexpr double BUTTON_SCALE = 2.0;

	this->ResumeButton = Button({ 0, 0, 445, 100 }, { 515.0, 550.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->FFFFont, "RESUME",
		[=]() { this->ReturnToGameplay = true; }, 90);

	this->QuitButton = Button({ 0, 0, 445, 100 }, { 515.0, 322.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->FFFFont, "EXIT", 
		[=]() { this->ReturnToMainMenu = true; }, 90);
}

void PauseGameMenu::DestroyState() 
{ 
	this->ReturnToGameplay = false; 
	this->ReturnToMainMenu = false;
}

void PauseGameMenu::UpdateTick(const double& DeltaTime)
{
	// Handle fade transition of pause menu interface
	if (this->ReturnToGameplay)
	{
		Effects::PlayFadeEffect(TransitionType::HIDE, this->InterfaceOpacity, 0.002, DeltaTime);
		if (this->InterfaceOpacity <= 0.0)
			this->PopState();
	}
	else
		Effects::PlayFadeEffect(TransitionType::REVEAL, this->InterfaceOpacity, 0.002, DeltaTime);

	if (this->ReturnToMainMenu)
	{
		Transition::PlayTransitionScreen(TransitionType::HIDE, DeltaTime);
		if (PostProcessing::GetSingleton().GetOpacity() == 0.0)
		{
			// Stop all audio and start the main menu state
			AudioPlayer::GetSingleton().StopAllAudio();
			ResourceLoading::GetGameState()->ThemeAudio = 
				AudioPlayer::GetSingleton().PlayAudio("GameFiles/Audio/TITLE_SCREEN.mp3", false, true);

			std::this_thread::sleep_for(2s);
			this->SwitchState(MainMenu::GetGameState());
		}
	}

	// Update the buttons
	this->ResumeButton.UpdateButton();
	this->QuitButton.UpdateButton();
}

void PauseGameMenu::RenderFrame() const
{
	// Render the button border box texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1000, 530 },
		{ 460.0, (this->SceneCamera.GetViewSize().y / 2.0) - 285, 1000.0, 530.0 }, *this->ButtonBoxTex, 0.0f, true, false,
		1.0f, (float)this->InterfaceOpacity);

	// Render the buttons
	this->ResumeButton.RenderButton((float)this->InterfaceOpacity);
	this->QuitButton.RenderButton((float)this->InterfaceOpacity);
}

PauseGameMenu* PauseGameMenu::GetGameState()
{
    static PauseGameMenu GameState;
    return &GameState;
}
