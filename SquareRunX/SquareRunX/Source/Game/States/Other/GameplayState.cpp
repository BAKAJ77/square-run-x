#include "GameplayState.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"
#include "Game/Utils/LevelLoader.h"

// Branching game states
#include "Game/States/Menus/PauseGameMenu.h"
#include "Game/States/Other/GameOverState.h"

void GameplayState::InitState()
{
	WindowFrame::GetSingleton().SetCursorState(CursorState::LOCK);
	this->UpdateAfterPause = false;

	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");
	this->FFFFont = ResourceLoading::GetGameState()->GetFont("FFF-Forwa");

	// Get the textures needed
	this->IntroTransitionTex1 = ResourceLoading::GetGameState()->GetTexture("Transition-1");
	this->IntroTransitionTex2 = ResourceLoading::GetGameState()->GetTexture("Transition-2");
	this->IntroTransitionTex3 = ResourceLoading::GetGameState()->GetTexture("Transition-3");

	// Initialize other stuff
	this->IntroTransitionDest1 = { 0.0, -1400.0, this->SceneCamera.GetViewSize().x, 1400.0 };
	this->IntroTransitionDest2 = { this->SceneCamera.GetViewSize().x + 600, 0.0, 600.0, 
		this->SceneCamera.GetViewSize().y };
	this->IntroTransitionDest3 = { -60.0, -600.0, this->SceneCamera.GetViewSize().x, 600.0 };

	this->CurrentLevel = ResourceLoading::GetGameState()->GetLevelMap(this->CurrentGameSave.Level, this->CurrentGameSave.Act);
	this->PlayerController = PlayerEntity(this->SceneCamera, 
		this->CurrentLevel->GetPlayerCheckpoints()[this->CurrentGameSave.CheckpointIndex], { 64, 64 }, 1.0, 0.0075, 1.75);

	this->PlayerController.SetLivesCounter(this->CurrentGameSave.NumLives);
	this->PlayerController.SetCurrentHealth(this->CurrentGameSave.CurrentHealth);
	this->PlayerController.SetMaximumHealth(this->CurrentGameSave.MaxHealth);

	this->ThemeAudio = AudioPlayer::GetSingleton().PlayAudio(this->CurrentLevel->GetThemeAudioPath().c_str(), true, true, 
		true);
	this->LevelTimeElapsedMS = this->CurrentGameSave.LevelTimePassedMS;
}

void GameplayState::DestroyState()
{
	this->AudioVolume = 1.0;
	this->CounterTextOpacity = 0.0;
	this->IntroTextOpacity = 0.0;
	this->SceneBrightness = 1.0;

	this->IntroComplete = false;
	this->IntroCurrentlyDisplayed = false;
	this->ShouldRenderState = false;
	this->ScreenFadedAway = false;
	this->GameOver = false;
	this->Paused = false;
}

void GameplayState::ResumeState()
{
	this->Paused = false;
	this->AudioVolume = 1.0;

	WindowFrame::GetSingleton().SetCursorState(CursorState::LOCK);
}

void GameplayState::UpdateTick(const double& DeltaTime)
{
	if (!this->IntroComplete)
	{
		// Handle the level gameplay intro transition
		this->HandleIntroTransition(DeltaTime);
	}
	else
	{
		this->PlayerController.GetAnimatedSprite().SetAnimationPaused(this->Paused);

		// Check if player pressed ESCAPE to pause game
		if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_ESCAPE) && !this->PlayerController.IsDestroyed())
			this->Paused = true;

		if (!this->Paused)
		{
			this->LevelTimeElapsedMS += DeltaTime;

			// Update level scene entities/objects
			this->PlayerController.UpdateEntity(this->CurrentLevel, DeltaTime);
			this->CurrentLevel->GetBackground().UpdateParallaxState(
				{
					glm::dvec2(-0.00125 * this->PlayerController.GetCurrentVelocity().x, 0.0),
					glm::dvec2(-0.0025 * this->PlayerController.GetCurrentVelocity().x, 0.0),
					glm::dvec2(std::min(-0.0075 * this->PlayerController.GetCurrentVelocity().x, -0.0075), 0.0),
					glm::dvec2(-0.01 * this->PlayerController.GetCurrentVelocity().x, 0.0),
					glm::dvec2(-0.02 * this->PlayerController.GetCurrentVelocity().x, 0.0),
					glm::dvec2(-0.04 * this->PlayerController.GetCurrentVelocity().x, 0.0)
				}, this->SceneCamera, DeltaTime);

			Effects::PlayFadeEffect(TransitionType::REVEAL, this->SceneBrightness, 0.002, DeltaTime);
			Effects::PlayFadeEffect(TransitionType::REVEAL, this->CounterTextOpacity, 0.0015, DeltaTime);
		}
		else
		{
			// Do paused scene fade effect
			Effects::PlayFadeEffect(TransitionType::HIDE, this->CounterTextOpacity, 0.0015, DeltaTime);
			if (this->CounterTextOpacity <= 0.0)
			{
				if (this->SceneBrightness <= 0.3)
				{
					this->RenderAfterPause = true;
					this->PushState(PauseGameMenu::GetGameState());
				}
				else
					Effects::PlayFadeEffect(TransitionType::HIDE, this->SceneBrightness, 0.002, DeltaTime);
			}
		}
	}

	// Check if player's health is empty, if so then stop gameplay and respawn player at last checkpoint
	if (this->PlayerController.IsHealthEmpty() && this->IntroComplete)
	{
		if (!this->PlayerController.IsDestroyed())
		{
			this->PlayerController.DestroyEntity();
			if(this->PlayerController.GetLivesCounter() == 0)
				this->GameOver = true;
		}

		if (this->GameOver)
		{
			Transition::PlayTransitionScreen(TransitionType::HIDE, DeltaTime);
			
			if (PostProcessing::GetSingleton().GetOpacity() == 0.0 && this->GameOver)
			{
				Effects::PlayFadeEffect(TransitionType::HIDE, this->AudioVolume, 0.001, DeltaTime);
				this->ThemeAudio->setVolume((float)this->AudioVolume);

				static double ElapsedTime = 0.0;
				if (ElapsedTime >= 1500.0 && this->AudioVolume >= 0.0)
				{
					this->RenderAfterPause = false;

					this->PushState(GameOverState::GetGameState());
					this->ThemeAudio->setIsPaused();
					this->GameOver = false;
					
					ElapsedTime = 0.0;
				}
				else
					ElapsedTime += DeltaTime;
			}
		}

		if (!this->GameOver)
			this->HandleIntroTransition(DeltaTime, true, 2000.0, true);
	}

	if (this->IntroCurrentlyDisplayed && this->PlayerController.IsHealthEmpty())
	{
		this->PlayerController.RespawnEntity(this->CurrentLevel);
		if (this->PlayerController.GetLivesCounter() < 0)
			this->PlayerController.SetLivesCounter(3);
	}
}

void GameplayState::RenderFrame() const
{
	if (this->ShouldRenderState && !this->ScreenFadedAway)
	{
		this->CurrentLevel->RenderTiles(this->SceneCamera, this->SceneBrightness);
		this->PlayerController.RenderEntity((float)this->SceneBrightness);

		if(this->CounterTextOpacity > 0.0)
			this->RenderCounters();
	}

	if (!this->IntroComplete)
	{
		// Render transition textures
		GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1400 }, this->IntroTransitionDest1, 
			*this->IntroTransitionTex1, 0.0f, true);
		GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 600 }, this->IntroTransitionDest3,
			*this->IntroTransitionTex3, 0.0f, true);
		GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 600, 1080 }, this->IntroTransitionDest2, 
			*this->IntroTransitionTex2, 0.0f, true);

		// Render the level description text
		GraphicsRenderer::GetSingleton().RenderText({ 150, 705 }, 120, this->CurrentLevel->GetLevelName(), 
			*this->ArialRoundedFont, { 0.0f, 1.0f, 0.0f, this->IntroTextOpacity }, true);

		GraphicsRenderer::GetSingleton().RenderText({ 150, 605 }, 80, "LEVEL " + 
			std::to_string(this->CurrentLevel->GetLevelIndex()) + " ACT " + std::to_string(this->CurrentLevel->GetActIndex()),
			*this->ArialRoundedFont, { 1.0f, 0.0f, 0.0f, this->IntroTextOpacity }, true);
	}
}

void GameplayState::RenderCounters() const
{
	// Generate the counter text strings
	const std::string SCORE_TEXT_STR = "SCORE: " + std::to_string(this->CurrentGameSave.Score);
	const std::string LIVES_TEXT_STR = "LIVES: " + std::to_string(this->PlayerController.GetLivesCounter());
	const std::string TIME_TEXT_STR = "TIME: " + std::to_string((int)(this->LevelTimeElapsedMS / 1000.0) / 60) + "mins " +
		std::to_string((int)(this->LevelTimeElapsedMS / 1000.0) % 60) + "secs";

	// Calculate the required widths of the text strings
	const float SCORE_TEXT_WIDTH = GraphicsRenderer::GetSingleton().GetTextSize(SCORE_TEXT_STR, *this->FFFFont, 72).x;
	const float LIVES_TEXT_WIDTH = GraphicsRenderer::GetSingleton().GetTextSize(LIVES_TEXT_STR, *this->FFFFont, 72).x;

	// Render the counters
	constexpr float TEXT_OFFSET = 25;

	GraphicsRenderer::GetSingleton().RenderText({ (1920.0f - SCORE_TEXT_WIDTH) - TEXT_OFFSET, 965 }, 72, SCORE_TEXT_STR,
		*this->FFFFont, { 0.0f, 0.0f, 1.0f, (float)this->CounterTextOpacity }, true);
	GraphicsRenderer::GetSingleton().RenderText({ (1920.0f - LIVES_TEXT_WIDTH) - TEXT_OFFSET, 20 }, 72, LIVES_TEXT_STR,
		*this->FFFFont, { 0.0f, 0.0f, 1.0f, (float)this->CounterTextOpacity }, true);
	GraphicsRenderer::GetSingleton().RenderText({ TEXT_OFFSET, 965 }, 72, TIME_TEXT_STR, *this->FFFFont, 
		{ 0.0f, 0.0f, 1.0f, (float)this->CounterTextOpacity }, true);
}

GameplayState* GameplayState::GetGameState(const GameSave& CurrentGameSave)
{
	static GameplayState GameState;
	GameState.SetCurrentGameSave(CurrentGameSave);

	return &GameState;
}

void GameplayState::SetCurrentGameSave(const GameSave& CurrentGameSave) { this->CurrentGameSave = CurrentGameSave; }

void GameplayState::HandleIntroTransition(const double& DeltaTime, bool ResetIntroState, double DelayTime, bool FadeAway)
{
	static double ElapsedTime = 0.0, Delay = 0.0;
	if (DelayTime > 0.0)
		Delay = DelayTime;

	if (ElapsedTime >= Delay)
	{
		if (GameStateManager::GetSingleton().HasCurrentStateStarted())
		{
			static constexpr float TRANSITION_SPEED = 5.5f, OPACITY_CHANGE_RATE = 0.005f;
			static bool FinishedDisplay = false, PreStageComplete = false, StageOneComplete = false, StageTwoComplete = false,
				ShouldFade = false, CompletedFadeAway = false;

			// Do fade animation (not necessary)
			if(FadeAway)
				ShouldFade = true;

			if (ShouldFade)
			{
				if (PostProcessing::GetSingleton().GetOpacity() > 0.0 && this->AudioVolume > 0.0 && !CompletedFadeAway)
				{
					Transition::PlayTransitionScreen(TransitionType::HIDE, DeltaTime);
					Effects::PlayFadeEffect(TransitionType::HIDE, this->AudioVolume, 0.001, DeltaTime);

					this->ThemeAudio->setVolume((float)this->AudioVolume);
				}
				else
				{
					static double ElapsedFadeTime = 0.0;
					if (ElapsedFadeTime >= 500.0)
					{
						Transition::PlayTransitionScreen(TransitionType::REVEAL, DeltaTime);
						if (PostProcessing::GetSingleton().GetOpacity() >= 1.0)
						{
							this->AudioVolume = 1.0;

							this->ThemeAudio->setVolume((float)this->AudioVolume);
							this->ThemeAudio->setIsPaused();
							this->ThemeAudio->setPlayPosition(0);

							ShouldFade = false;
							ElapsedFadeTime = 0.0;
						}
					}

					CompletedFadeAway = true;
					this->ScreenFadedAway = true;
					this->CounterTextOpacity = 0.0;

					ElapsedFadeTime += DeltaTime;
				}
			}
			else
				CompletedFadeAway = false;

			// Do the main intro animation
			if (FinishedDisplay && !ShouldFade)
			{
				if (!PreStageComplete)
				{
					this->IntroTextOpacity = (float)std::max(this->IntroTextOpacity - (OPACITY_CHANGE_RATE * DeltaTime), 0.0);

					if (this->IntroTextOpacity <= 0.0f)
						PreStageComplete = true;
				}
				else if (!StageOneComplete)
				{
					if (this->IntroTransitionDest3.y <= -this->IntroTransitionDest3.h)
						StageOneComplete = true;
					else
						this->IntroTransitionDest3.y -= (int)(TRANSITION_SPEED * DeltaTime);
				}
				else if (!StageTwoComplete)
				{
					if (this->IntroTransitionDest2.x >= SceneCamera.GetViewSize().x)
						StageTwoComplete = true;
					else
						this->IntroTransitionDest2.x += (int)(TRANSITION_SPEED * DeltaTime);
				}
				else
				{
					if (this->IntroTransitionDest1.y <= -this->IntroTransitionDest1.h)
					{
						FinishedDisplay = false;
						StageOneComplete = false;
						StageTwoComplete = false;
						PreStageComplete = false;

						this->IntroComplete = true;
						this->LevelTimeElapsedMS = this->CurrentGameSave.LevelTimePassedMS;
					}
					else
						this->IntroTransitionDest1.y -= (int)(TRANSITION_SPEED * DeltaTime);
				}
			}
			else if(!ShouldFade)
			{
				if (!PreStageComplete)
				{
					this->IntroTransitionDest1.y += (int)(TRANSITION_SPEED * DeltaTime);
					if (this->IntroTransitionDest1.y + 100 >= 0)
					{
						this->ThemeAudio->setIsPaused(false);

						this->IntroTransitionDest1.y = -100;
						PreStageComplete = true;
					}
				}
				else if (!StageOneComplete)
				{
					this->IntroTransitionDest2.x -= (int)(TRANSITION_SPEED * DeltaTime);
					if ((this->IntroTransitionDest2.x + this->IntroTransitionDest2.w) - 175 <= SceneCamera.GetViewSize().x)
					{
						this->IntroTransitionDest2.x = (int)this->SceneCamera.GetViewSize().x - (this->IntroTransitionDest2.w - 175);
						StageOneComplete = true;
					}
				}
				else if (!StageTwoComplete)
				{
					this->IntroTransitionDest3.y += (int)(TRANSITION_SPEED * DeltaTime);
					if (this->IntroTransitionDest3.y + 310 >= 0)
					{
						this->IntroTransitionDest3.y = -310;
						StageTwoComplete = true;
					}
				}
				else
				{
					this->IntroTextOpacity = (float)std::min(this->IntroTextOpacity + (OPACITY_CHANGE_RATE * DeltaTime), 1.0);

					if (this->IntroTextOpacity >= 1.0f)
					{
						static double ElapsedTime = 0.0;
						ElapsedTime += DeltaTime;

						if (ElapsedTime >= 1500.0)
						{
							FinishedDisplay = true;
							StageOneComplete = false;
							StageTwoComplete = false;
							PreStageComplete = false;

							this->ScreenFadedAway = false;
							this->ShouldRenderState = true;
							ElapsedTime = 0.0;
						}
					}
				}
			}

			if (ResetIntroState)
			{
				FinishedDisplay = false;
				PreStageComplete = false;
				StageOneComplete = false;
				StageTwoComplete = false;

				this->IntroComplete = false;
			}

			this->IntroCurrentlyDisplayed = FinishedDisplay;
		}

		ElapsedTime = 0.0;
		Delay = 0.0;
	}
	else
		ElapsedTime += DeltaTime;
}