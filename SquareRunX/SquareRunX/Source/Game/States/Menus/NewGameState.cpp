#include "NewGameMenu.h"
#include <GLFW/glfw3.h>

#include "Game/States/Other/ResourceLoading.h"
#include "Game/Utils/TransitionHandler.h"

// Branching game states
#include "Game/States/Other/GameplayState.h"

namespace TextBoxHandling
{
	std::string EnteredString;

	void PollTextboxInput(GLFWwindow* Window, uint32_t Char)
	{
		if ((EnteredString.size() < 12) &&
			((Char >= 65 && Char <= 90) || (Char >= 97 && Char <= 122)))
		{
			EnteredString.push_back(Char);
			std::transform(EnteredString.begin(), EnteredString.end(), EnteredString.begin(), ::toupper);
		}
	}
}

void NewGameMenu::InitState()
{
	this->UpdateAfterPause = false;
	TextBoxHandling::EnteredString.clear();

	// Get the fonts needed
	this->ArialRoundedFont = ResourceLoading::GetGameState()->GetFont("Arial-Rounded");
	this->SansSerifShadedFont = ResourceLoading::GetGameState()->GetFont("Sans-Serif-Shaded");

	// Get the textures needed
	this->TextBoxTex = ResourceLoading::GetGameState()->GetTexture("Text-Box");

	// Setup the buttons needed
	constexpr double BUTTON_SCALE = 1.2;
	static bool Continuing = false;

	this->ConfirmButton = Button({ 0, 0, 445, 100 }, { 180.0, 200.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "CONFIRM",
		[&]() 
		{ 
			if (TextBoxHandling::EnteredString.size() >= 2 && !Continuing)
			{
				GameSave NewSave;
				NewSave.PlayerName = TextBoxHandling::EnteredString;

				this->SwitchState(GameplayState::GetGameState(NewSave));
				Continuing = true;
			}
		}, 60);

	this->CancelButton = Button({ 0, 0, 445, 100 }, { 775.0, 200.0, (445 * BUTTON_SCALE), (100 * BUTTON_SCALE) },
		*ResourceLoading::GetGameState()->GetTexture("Button-Img"), *this->ArialRoundedFont, "CANCEL",
		[&]() 
		{ 
			if (!Continuing)
				this->PopState();
		}, 60);

	// Set the text polling callback function
	glfwSetCharCallback(WindowFrame::GetSingleton().GetLibraryWindow(), TextBoxHandling::PollTextboxInput);
}

void NewGameMenu::DestroyState() {}

void NewGameMenu::UpdateTick(const double& DeltaTime)
{
	// Update the buttons
	this->ConfirmButton.UpdateButton();
	this->CancelButton.UpdateButton();

	// Check if the backspace key is pressed so the last character can be erased
	this->PopLastCharacter(DeltaTime);
}

void NewGameMenu::RenderFrame() const
{
	Rect SourceRect, DestinationRect;

	// Render the new game menu title
	GraphicsRenderer::GetSingleton().RenderText({ 40, 940 }, 100, "CREATE NEW GAME", *this->SansSerifShadedFont);

	// Render the text box
	SourceRect = { 0, 0, 1370, 380 };
	DestinationRect = { 60.0, (this->SceneCamera.GetViewSize().y / 2) - (SourceRect.h / 2), SourceRect.w, SourceRect.h };

	GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->TextBoxTex, 0.0f, true);

	// Render text description
	glm::vec3 TextColor;
	TextBoxHandling::EnteredString.size() < 2 ? TextColor = { 1.0f, 0.0f, 0.0f } : TextColor = { 0.0f, 1.0f, 0.0f };

	GraphicsRenderer::GetSingleton().RenderText({ 320, (int)(this->SceneCamera.GetViewSize().y / 2.0f) + 95 }, 60,
		"ENTER YOUR NAME BELOW:", *this->ArialRoundedFont, glm::vec4(TextColor, 1.0f));

	// Render the text in the input text box
	const float TEXT_WIDTH = GraphicsRenderer::GetSingleton().GetTextSize(TextBoxHandling::EnteredString, 
		*this->ArialRoundedFont, 72).x;

	GraphicsRenderer::GetSingleton().RenderText({ (int)((60 + (SourceRect.w / 2)) - (TEXT_WIDTH / 2.0f)), 
		(int)(this->SceneCamera.GetViewSize().y / 2.0f) - 10 }, 
		72, TextBoxHandling::EnteredString, *this->ArialRoundedFont);

	// Render the buttons
	this->ConfirmButton.RenderButton();
	this->CancelButton.RenderButton();
}

NewGameMenu* NewGameMenu::GetGameState()
{
	static NewGameMenu GameState;
	return &GameState;
}

void NewGameMenu::PopLastCharacter(const double& DeltaTime) const
{
	static double ElapsedTime = 0.0;
	if (ElapsedTime >= 125.0)
	{
		if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_BACKSPACE) && !TextBoxHandling::EnteredString.empty())
		{
			TextBoxHandling::EnteredString.pop_back();
			ElapsedTime = 0.0;
		}
	}

	ElapsedTime += DeltaTime;
}