#include "ButtonObject.h"
#include "Engine/Core/InputHandler.h"
#include "Engine/Core/WindowFrame.h"

#include "Engine/Core/AudioPlayer.h"

namespace 
{
	constexpr float BUTTON_CLICK_COOLDOWN = 500.0;
}

Button::Button() :
	ButtonBKGTex(nullptr), ButtonTextFont(nullptr), BrightnessThreshold(1.0f), FontSize(48), CursorEntered(false)
{}

Button::Button(const Rect& TextureAtlas, const Rect& Destination, const Texture& BKGTexture, const Font& TextFont, 
	const std::string& Text, const std::function<void()>& Function, uint32_t FontSize, const glm::vec3& TextColor) :
	TextureAtlas(TextureAtlas), ButtonBKGTex(&BKGTexture), ButtonTextFont(&TextFont), ExecutableFunction(Function),
	ButtonText(Text), BrightnessThreshold(1.0f), Destination(Destination), FontSize(FontSize), TextColor(TextColor),
	CursorEntered(false)
{}

Button::~Button() {}

void Button::UpdateButton() 
{
	const glm::vec2& CURSOR_POS = InputHandler::GetSingleton().GetMousePosition();
	
	// Check if the mouse cursor is intersecting the button
	if (CURSOR_POS.x >= this->Destination.x && CURSOR_POS.x <= this->Destination.x + this->Destination.w &&
		CURSOR_POS.y >= this->Destination.y && CURSOR_POS.y <= this->Destination.y + this->Destination.h)
	{
		static double PrevTime = 0.0;

		if (InputHandler::GetSingleton().WasMouseButtonClicked(InputCode::MOUSE_BUTTON_LEFT) &&
			WindowFrame::GetSingleton().GetTick() - PrevTime >= BUTTON_CLICK_COOLDOWN)
		{
			this->ExecutableFunction();
			PrevTime = WindowFrame::GetSingleton().GetTick();
		}

		if (!this->CursorEntered)
		{
			AudioPlayer::GetSingleton().SetAudioVolume(0.4f);
			AudioPlayer::GetSingleton().PlayAudio("GameFiles/Audio/SOLID.wav");
			AudioPlayer::GetSingleton().SetAudioVolume(1.0f);

			this->CursorEntered = true;
		}

		this->BrightnessThreshold = 1.5f;
	}
	else
	{
		this->BrightnessThreshold = 1.0f;
		this->CursorEntered = false;
	}
}

void Button::RenderButton(float Opacity, int TextOffset) const
{
	// Render the background of button
	GraphicsRenderer::GetSingleton().RenderQuad(this->TextureAtlas, this->Destination, *this->ButtonBKGTex, 0.0f, true, false,
		this->BrightnessThreshold, Opacity);

	// Calculate where the text will be positioned
	const glm::vec2 TEXT_SIZE = GraphicsRenderer::GetSingleton().GetTextSize(this->ButtonText, *this->ButtonTextFont, 
		this->FontSize);
	const glm::vec2 TEXT_POS = { ((this->Destination.x + (this->Destination.w / 2)) - (TEXT_SIZE.x / 2)) + TextOffset,
		(this->Destination.y + (this->Destination.h / 2)) - (TEXT_SIZE.y / 2) };

	// Render the text on top of button
	GraphicsRenderer::GetSingleton().RenderText(TEXT_POS, this->FontSize, this->ButtonText, *this->ButtonTextFont, 
		{ this->TextColor, Opacity });
}