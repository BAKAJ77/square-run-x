#include "ButtonObject.h"
#include "Engine/Core/InputHandler.h"

Button::Button() :
	ButtonBKGTex(nullptr), ButtonTextFont(nullptr), BrightnessThreshold(1.0f)
{}

Button::Button(const Rect& TextureAtlas, const Rect& Destination, const Texture& BKGTexture, const Font& TextFont, 
	const std::string& Text, const std::function<void()>& Function) :
	TextureAtlas(TextureAtlas), ButtonBKGTex(&BKGTexture), ButtonTextFont(&TextFont), ExecutableFunction(Function),
	ButtonText(Text), BrightnessThreshold(1.0f), Destination(Destination)
{}

Button::~Button() {}

void Button::UpdateButton() 
{
	const glm::vec2& CURSOR_POS = InputHandler::GetSingleton().GetMousePosition();
	
	// Check if the mouse cursor is intersecting the button
	if (CURSOR_POS.x >= this->Destination.x && CURSOR_POS.x <= this->Destination.x + this->Destination.w &&
		CURSOR_POS.y >= this->Destination.y && CURSOR_POS.y <= this->Destination.y + this->Destination.h)
	{
		if (InputHandler::GetSingleton().WasMouseButtonClicked(InputCode::MOUSE_BUTTON_LEFT))
			this->ExecutableFunction();

		this->BrightnessThreshold = 1.5f;
	}
	else
		this->BrightnessThreshold = 1.0f;
}

void Button::RenderButton(float Opacity) const
{
	// Render the background of button
	GraphicsRenderer::GetSingleton().RenderQuad(this->TextureAtlas, this->Destination, *this->ButtonBKGTex, 0.0f, true, false,
		this->BrightnessThreshold, Opacity);

	// Calculate where the text will be positioned
	constexpr int OFFSET_WEIGHT = 10;
	const glm::vec2 TEXT_SIZE = GraphicsRenderer::GetSingleton().GetTextSize(this->ButtonText, *this->ButtonTextFont, 48);
	const glm::vec2 TEXT_POS = { ((this->Destination.x + (this->Destination.w / 2)) - (TEXT_SIZE.x / 2)) + OFFSET_WEIGHT,
		(this->Destination.y + (this->Destination.h / 2)) - (TEXT_SIZE.y / 2) };

	// Render the text on top of button
	GraphicsRenderer::GetSingleton().RenderText(TEXT_POS, 48, this->ButtonText, *this->ButtonTextFont, 
		{ glm::vec3(0.0f), Opacity });
}