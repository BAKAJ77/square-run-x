#include "MainMenu.h"

// Branching game states
#include "PlayMenu.h"
#include "LeaderboardMenu.h"
#include "ControlsMenu.h"

#include "Game/States/Other/ResourceLoading.h"

void MainMenu::InitState() 
{
	this->UpdateAfterPause = false;

	// Get the fonts needed
	this->DiscoveryFont = ResourceLoading::GetGameState()->GetFont("Discovery");
	
	// Load the parallax background
	this->MenuBackground = ParallaxBKG(
		{
			ResourceLoading::GetGameState()->GetTexture("Forest-Sky"),
			ResourceLoading::GetGameState()->GetTexture("Forest-Mountains"),
			ResourceLoading::GetGameState()->GetTexture("Forest-Clouds"),
			ResourceLoading::GetGameState()->GetTexture("Forest-Far"),
			ResourceLoading::GetGameState()->GetTexture("Forest-Middle"),
			ResourceLoading::GetGameState()->GetTexture("Forest-Close"),
		}, 6.75f);

	// Setup the menu buttons
	const Rect SOURCE_RECT = { 0, 0, 445, 100 };

	Rect DestinationRect = { (this->SceneCamera.GetViewSize().x / 2.0) - 222.0, 575.0, SOURCE_RECT.w, SOURCE_RECT.h };
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
}

void MainMenu::DestroyState() {}

void MainMenu::UpdateTick(const double& DeltaTime) 
{
	// Update parallax menu background
	this->MenuBackground.UpdateParallaxState(
		{ glm::dvec2(-0.00625, 0.0), glm::dvec2(-0.0125, 0.0), glm::dvec2(-0.025, 0.0), glm::dvec2(-0.05, 0.0), 
		glm::dvec2(-0.1, 0.0), glm::dvec2(-0.2, 0.0) }, this->SceneCamera, DeltaTime);

	// Handle the menu scene scrolling effect
	constexpr float SCROLL_SPEED = 0.05f;
	this->SceneCamera.SetPosition({ this->SceneCamera.GetPosition().x + (SCROLL_SPEED * DeltaTime), 0.0f });

	// Update the button states
	this->PlayButton.UpdateButton();
	this->LeaderboardButton.UpdateButton();
	this->ControlsButton.UpdateButton();
	this->ExitButton.UpdateButton();
}

void MainMenu::RenderFrame() const
{
	// Render the parallax background
	this->MenuBackground.RenderParallax();

	// Render the menu title
	GraphicsRenderer::GetSingleton().RenderText({ 470, 800 }, 100, "SQUARE RUN X", *this->DiscoveryFont);

	// Render the menu buttons
	this->PlayButton.RenderButton();
	this->LeaderboardButton.RenderButton(1.0f, 15);
	this->ControlsButton.RenderButton();
	this->ExitButton.RenderButton();
}

MainMenu* MainMenu::GetGameState()
{
	static MainMenu State;
	return &State;
}
