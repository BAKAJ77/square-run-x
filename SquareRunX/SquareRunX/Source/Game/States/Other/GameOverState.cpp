#include "GameOverState.h"
#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

#include <thread>
#include <chrono>

// Branching game states
#include "Game/States/Menus/MainMenu.h"

using namespace std::chrono_literals;

void GameOverState::InitState()
{
	this->StateCurrent = true;
	this->QuitGame = false;

	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");
	this->DiscoveryFont = ResourceLoading::GetGameState()->GetFont("Discovery");
	this->FFFFont = ResourceLoading::GetGameState()->GetFont("FFF-Forwa");

	// Get the textures needed
	this->GameOverFadeTex = ResourceLoading::GetGameState()->GetTexture("Game-Over");

	// Load the game over audio
	this->GameOverAudio = AudioPlayer::GetSingleton().PlayAudio("GameFiles/Audio/GAME_OVER.mp3", false, true);
}

void GameOverState::DestroyState() 
{
	this->GameOverTextOpacity = 0.0;
	this->ActionTextOpacity = 0.0;
}

void GameOverState::UpdateTick(const double& DeltaTime)
{
	// Handles the reveal/hide effect of the game over game state
	if (this->StateCurrent && PostProcessing::GetSingleton().GetOpacity() < 1.0)
		Transition::PlayTransitionScreen(TransitionType::REVEAL, DeltaTime);
	else if (!this->StateCurrent)
	{
		Transition::PlayTransitionScreen(TransitionType::HIDE, DeltaTime);
		if (PostProcessing::GetSingleton().GetOpacity() == 0.0)
		{
			if (this->QuitGame)
			{
				ResourceLoading::GetGameState()->ThemeAudio =
					AudioPlayer::GetSingleton().PlayAudio("GameFiles/Audio/TITLE_SCREEN.mp3", false, true);

				std::this_thread::sleep_for(2s);
				this->SwitchState(MainMenu::GetGameState());
			}
			else
				this->PopState();
		}
	}

	// Update text opacity
	Effects::PlayFadeEffect(TransitionType::REVEAL, this->GameOverTextOpacity, 0.00025, DeltaTime);
	if (this->GameOverAudio->isFinished())
	{
		Effects::PlayFadeEffect(TransitionType::REVEAL, this->ActionTextOpacity, 0.0025, DeltaTime);

		// Check for key input on whether to try again or quit
		if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_ENTER) && this->StateCurrent)
			this->StateCurrent = false;
		else if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_ESCAPE) && this->StateCurrent)
		{
			this->StateCurrent = false;
			this->QuitGame = true;
		}
	}
}

void GameOverState::RenderFrame() const
{
	// Render the background texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1080 }, { 0, 0, 1920, 1080 }, *this->GameOverFadeTex, 0.0, true);

	// Render the text
	GraphicsRenderer::GetSingleton().RenderText({ 160, 500 }, 200, "GAME OVER", *this->DiscoveryFont, 
		{ 0.2, 0.0, 0.0, this->GameOverTextOpacity }, true);
	GraphicsRenderer::GetSingleton().RenderText({ 230, 50 }, 50, "Press Enter to try again, Press Escape to quit", 
		*this->FFFFont, { 0.3, 0.3, 0.3, this->ActionTextOpacity });
}

GameOverState* GameOverState::GetGameState()
{
	static GameOverState Singleton;
	return &Singleton;
}
