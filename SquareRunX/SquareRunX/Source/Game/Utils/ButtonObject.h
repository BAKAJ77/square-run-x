#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"
#include <functional>

class Button
{
private:
	const Texture* ButtonBKGTex;
	const Font* ButtonTextFont;
	
	Rect TextureAtlas, Destination;
	std::string ButtonText;
	std::function<void()> ExecutableFunction;

	float BrightnessThreshold;
	size_t FontSize;
public:
	Button();
	Button(const Rect& TextureAtlas, const Rect& Destination, const Texture& BKGTexture, const Font& TextFont, 
		const std::string& Text, const std::function<void()>& Function);
	~Button();

	void UpdateButton();
	void RenderButton(float Opacity = 1.0f) const;
};