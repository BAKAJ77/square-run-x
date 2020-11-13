#include "LeaderboardMenu.h"
#include "MainMenu.h"

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

namespace
{
	constexpr uint8_t LEADERBOARD_MAX = 3;
}

void LeaderboardMenu::InitState()
{
	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");
	this->SansSerifShadedFont = ResourceLoading::GetGameState()->GetFont("Sans-Serif-Shaded");

	// Get the textures needed
	this->LeaderboardSkinTex = ResourceLoading::GetGameState()->GetTexture("Leaderboard-Skin");
	this->TransitionTex1 = ResourceLoading::GetGameState()->GetTexture("Transition-1");
	this->TransitionTex2 = ResourceLoading::GetGameState()->GetTexture("Transition-2");

	// Setup the buttons needed
	this->BackButton = Button({ 0, 0, 445, 100 }, { 45, 30, 445, 100 },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "BACK",
		[=]() { this->EndOfState = true; this->StageOneComplete = false; this->PreStageComplete = false; });

	// Initialize other stuff
	this->TransitionDest1 = { 0.0, -1400.0, this->SceneCamera.GetViewSize().x, 1400.0 };
	this->TransitionDest2 = { this->SceneCamera.GetViewSize().x + 600, 0.0, 600.0, this->SceneCamera.GetViewSize().y };

	this->ExtractLeaderboardData();
}

void LeaderboardMenu::DestroyState()
{
	this->TransitionComplete = false;
	this->PreStageComplete = false;
	this->StageOneComplete = false;
	this->EndOfState = false;
}

void LeaderboardMenu::UpdateTick(const double& DeltaTime)
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

void LeaderboardMenu::RenderFrame() const
{
	// Render transition texture
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 1920, 1400 }, this->TransitionDest1, *this->TransitionTex1, 0.0f, true);
	GraphicsRenderer::GetSingleton().RenderQuad({ 0, 0, 600, 1080 }, this->TransitionDest2, *this->TransitionTex2, 0.0f, true);

	Rect SourceRect, DestinationRect;

	// Render the leaderboard menu title
	GraphicsRenderer::GetSingleton().RenderText({ 40, 940 }, 100, "LEADERBOARD", *this->SansSerifShadedFont,
		{ glm::vec3(0.0f), this->OpacityMultiplier });

	// Render the leaderboard skin texture
	SourceRect = { 0, 0, 1580, 700 };
	DestinationRect = { 30.0, 210.0, (SourceRect.w / 1.1f), (SourceRect.h / 1.1f) };

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->LeaderboardSkinTex, 0.0f, true, false,
		1.0f, this->OpacityMultiplier);

	// Render the details of players in leaderboard
	for (uint8_t i = 0; i < LEADERBOARD_MAX; i++)
	{
		this->RenderLeaderboardIndex(i);

		if (i != this->LeaderboardArray.size())
		{
			GraphicsRenderer::GetSingleton().RenderText({ 250, 725 - (195 * i) }, 48,
				"NAME: " + this->LeaderboardArray[i].Name, * this->ArialRoundedFont, 
				{ glm::vec3(0.0f), this->OpacityMultiplier });
			
			GraphicsRenderer::GetSingleton().RenderText({ 860, 725 - (195 * i) }, 48,
				"SCORE: " + std::to_string(this->LeaderboardArray[i].Score), *this->ArialRoundedFont,
				{ glm::vec3(0.0f), this->OpacityMultiplier });
		}
	}

	// Render the buttons
	this->BackButton.RenderButton(this->OpacityMultiplier);
}

LeaderboardMenu* LeaderboardMenu::GetGameState()
{
	static LeaderboardMenu GameState;
	return &GameState;
}

void LeaderboardMenu::RenderLeaderboardIndex(uint8_t Index) const
{
	switch (Index + 1)
	{
	case 1:
		GraphicsRenderer::GetSingleton().RenderText({ 98, 725 }, 48, "1ST", *this->ArialRoundedFont, 
			{ 1.0f, 0.843f, 0.0f, this->OpacityMultiplier }); // Gold color
		break;
	case 2:
		GraphicsRenderer::GetSingleton().RenderText({ 95, 530 }, 48, "2ND", *this->ArialRoundedFont, 
			{ 0.752f, 0.752f, 0.752f, this->OpacityMultiplier }); // Silver color
		break;
	case 3:
		GraphicsRenderer::GetSingleton().RenderText({ 95, 335 }, 48, "3RD", *this->ArialRoundedFont, 
			{ 0.804f, 0.498f, 0.196f, this->OpacityMultiplier }); // Bronze color
		break;
	}
}

void LeaderboardMenu::ExtractLeaderboardData()
{
	FileHandler::GetSingleton().OpenFile("LEADERBOARD", "GameFiles/Data/Leaderboard.dat");

	for (int i = 0; i < 3; i++)
	{
		// Extract the player data from leaderboard file
		const std::string LOADED_LINE = FileHandler::GetSingleton().ReadData("LEADERBOARD", i + 1).str();
		PlayerData Data;

		if (!LOADED_LINE.empty())
		{
			Data.Name = LOADED_LINE.substr(0, LOADED_LINE.find_first_of(','));
			Data.Score = std::stoi(LOADED_LINE.substr(LOADED_LINE.find_first_of(',') + 1, std::string::npos));
		}
		else
			break;

		this->LeaderboardArray.emplace_back(Data);
	}

	FileHandler::GetSingleton().CloseFile("LEADERBOARD");
}
