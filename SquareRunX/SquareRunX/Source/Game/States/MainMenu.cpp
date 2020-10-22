#include "MainMenu.h"
#include "ResourceLoading.h"

void MainMenu::InitState() 
{
	this->BackgroundTex = ResourceLoading::GetGameState()->GetTexture("Background-1");
	this->TileSheetTex = ResourceLoading::GetGameState()->GetTexture("Tile-Sheet");

	// Initialize/Fill the visible tiles vector
	for (int XPos = -80; XPos < (int)this->SceneCamera.GetViewSize().x; XPos += 80)
	{
		// Push the surface grass tile
		this->VisibleTiles.push_back({ glm::vec2(0, 0), glm::vec2(XPos, TileInfo::UNIFORM_INGAME_SIZE) });
		// Push the below-surface dirt tile
		this->VisibleTiles.push_back({ glm::vec2(TileInfo::UNIFORM_ATLAS_SIZE, 0), glm::vec2(XPos, 0) });
	}
}

void MainMenu::DestroyState() {}

void MainMenu::PauseState() {}

void MainMenu::ResumeState() {}

void MainMenu::UpdateTick(const float& DeltaTime) 
{
	// Handle game state transitioning
	if (!this->StartupComplete)
	{
		if (PostProcessing::GetSingleton().GetOpacity() == 1.0f)
			this->StartupComplete = true;
		else
			this->HandleTransitions(TransitionType::REVEAL);
	}

	// Update the visible tile positions to give an infinite flat level look
	for (auto& TileObj : this->VisibleTiles)
	{
		if (TileObj.Destination.x + TileObj.Destination.w < this->SceneCamera.GetPosition().x)
			TileObj.Destination.x = (int)(this->SceneCamera.GetPosition().x + this->SceneCamera.GetViewSize().x);
	}

	// Handle the menu scene scrolling effect
	constexpr float SCROLL_SPEED = 50.0f;
	this->SceneCamera.SetPosition({ this->SceneCamera.GetPosition().x + (SCROLL_SPEED * DeltaTime), 0.0f });
}

void MainMenu::RenderFrame() const
{
	// Render the main menu background
	Rect SourceRect = { 0, 0, 2560, 1440 };
	Rect DestinationRect = { 0, 0, (int)this->SceneCamera.GetViewSize().x, (int)this->SceneCamera.GetViewSize().y };

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->BackgroundTex, 0.0f, true);

	// Render the tiles
	for (const auto& TileObj : this->VisibleTiles)
		GraphicsRenderer::GetSingleton().RenderQuad(TileObj.TextureAtlas, TileObj.Destination, *this->TileSheetTex);
}

MainMenu* MainMenu::GetGameState()
{
	static MainMenu State;
	return &State;
}
