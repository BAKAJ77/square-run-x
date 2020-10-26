#include "MainMenu.h"

// Branching game states
#include "PlayMenu.h"
#include "LeaderboardMenu.h"
#include "ControlsMenu.h"

#include "Game/States/Other/ResourceLoading.h"

void MainMenu::InitState() 
{
	this->UpdateAfterPause = false;

	// Get the textures needed
	this->BackgroundTex = ResourceLoading::GetGameState()->GetTexture("Background-1");
	this->TileSheetTex = ResourceLoading::GetGameState()->GetTexture("Tile-Sheet");
	this->MenuTitleTex = ResourceLoading::GetGameState()->GetTexture("Menu-Title");

	// Setup the menu buttons
	const Rect SOURCE_RECT = { 0, 0, 445, 100 };

	Rect DestinationRect = { (int)(this->SceneCamera.GetViewSize().x / 2) - 222, 700, SOURCE_RECT.w, SOURCE_RECT.h };
	this->PlayButton = Button(SOURCE_RECT, DestinationRect, *ResourceLoading::GetGameState()->GetTexture("Button-Img"),
		*ResourceLoading::GetGameState()->GetFont("Arial-Rounded"), "PLAY", 
		[=]() { this->PushState(PlayMenu::GetGameState()); });

	DestinationRect.y -= 125;
	this->LeaderboardButton = Button(SOURCE_RECT, DestinationRect, *ResourceLoading::GetGameState()->GetTexture("Button-Img"),
		*ResourceLoading::GetGameState()->GetFont("Arial-Rounded"), "LEADERBOARD", 
		[=]() { this->PushState(LeaderboardMenu::GetGameState()); });

	DestinationRect.y -= 125;
	this->ControlsButton = Button(SOURCE_RECT, DestinationRect, *ResourceLoading::GetGameState()->GetTexture("Button-Img"),
		*ResourceLoading::GetGameState()->GetFont("Arial-Rounded"), "CONTROLS", 
		[=]() { this->PushState(ControlsMenu::GetGameState()); });

	DestinationRect.y -= 125;
	this->ExitButton = Button(SOURCE_RECT, DestinationRect, *ResourceLoading::GetGameState()->GetTexture("Button-Img"),
		*ResourceLoading::GetGameState()->GetFont("Arial-Rounded"), "EXIT", [=]() { this->PopState(); });

	// Initialize/Fill the visible tiles vector
	for (int XPos = -TileInfo::UNIFORM_INGAME_SIZE; XPos < (int)this->SceneCamera.GetViewSize().x;
		XPos += TileInfo::UNIFORM_INGAME_SIZE)
	{
		// Push the surface grass tile
		this->VisibleTiles.push_back({ glm::vec2(0, 0), glm::vec2(XPos, TileInfo::UNIFORM_INGAME_SIZE) });
		// Push the below-surface dirt tile
		this->VisibleTiles.push_back({ glm::vec2(TileInfo::UNIFORM_ATLAS_SIZE, 0), glm::vec2(XPos, 0) });
	}
}

void MainMenu::DestroyState() {}

void MainMenu::UpdateTick(const float& DeltaTime) 
{
	// Update the visible tile positions to give an infinite flat level look
	for (auto& TileObj : this->VisibleTiles)
	{
		if (TileObj.Destination.x + TileObj.Destination.w < this->SceneCamera.GetPosition().x)
		{
			int FurtherestTilePos = 0;
			for (const auto& OtherTileObj : this->VisibleTiles)
			{
				if (OtherTileObj.Destination.x + OtherTileObj.Destination.w > FurtherestTilePos && 
					OtherTileObj.Destination.y == TileObj.Destination.y)
					FurtherestTilePos = OtherTileObj.Destination.x + OtherTileObj.Destination.w;
			}

			TileObj.Destination.x = FurtherestTilePos;
		}
	}

	// Handle the menu scene scrolling effect
	constexpr float SCROLL_SPEED = 50.0f;
	this->SceneCamera.SetPosition({ this->SceneCamera.GetPosition().x + (SCROLL_SPEED * DeltaTime), 0.0f });

	// Update the button states
	this->PlayButton.UpdateButton();
	this->LeaderboardButton.UpdateButton();
	this->ControlsButton.UpdateButton();
	this->ExitButton.UpdateButton();
}

void MainMenu::RenderFrame() const
{
	// Render the main menu background
	Rect SourceRect = { 0, 0, 2560, 1440 };
	Rect DestinationRect = { 0, 0, (int)this->SceneCamera.GetViewSize().x, (int)this->SceneCamera.GetViewSize().y };

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->BackgroundTex, 0.0f, true);

	// Render the menu title
	SourceRect = { 0, 0, 792, 105 };
	DestinationRect = { (int)(this->SceneCamera.GetViewSize().x / 2), 925, SourceRect.w, SourceRect.h };
	DestinationRect.x -= (DestinationRect.w / 2);

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->MenuTitleTex, 0.0f, true);

	// Render the tiles
	for (const auto& TileObj : this->VisibleTiles)
		GraphicsRenderer::GetSingleton().RenderQuad(TileObj.TextureAtlas, TileObj.Destination, *this->TileSheetTex, 0.0f);

	// Render the menu buttons
	this->PlayButton.RenderButton();
	this->LeaderboardButton.RenderButton();
	this->ControlsButton.RenderButton();
	this->ExitButton.RenderButton();
}

MainMenu* MainMenu::GetGameState()
{
	static MainMenu State;
	return &State;
}
