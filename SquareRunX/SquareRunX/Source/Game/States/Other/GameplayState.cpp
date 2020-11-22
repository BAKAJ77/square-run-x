#include "GameplayState.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"
#include "Game/Utils/LevelLoader.h"

void GameplayState::InitState()
{
	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");

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
}

void GameplayState::DestroyState()
{
	this->IntroComplete = false;
	this->ShouldRenderState = false;
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
	}

	// Check if player's health is empty, if so then stop gameplay and respawn player at last checkpoint
	if (this->PlayerController.IsHealthEmpty() && this->IntroComplete)
	{
		this->HandleIntroTransition(DeltaTime, true, 2000.0, true);
		if(!this->PlayerController.IsDestroyed())
			this->PlayerController.DestroyEntity();
	}

	if (this->IntroCurrentlyDisplayed && this->PlayerController.IsHealthEmpty())
		this->PlayerController.RespawnEntity(this->CurrentLevel);
}

void GameplayState::RenderFrame() const
{
	if (this->ShouldRenderState && !this->ScreenFadedAway)
	{
		this->CurrentLevel->RenderTiles(this->SceneCamera);
		this->PlayerController.RenderEntity();
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
					if (ElapsedFadeTime >= 1000.0)
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

						if (ElapsedTime >= 2000.0)
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