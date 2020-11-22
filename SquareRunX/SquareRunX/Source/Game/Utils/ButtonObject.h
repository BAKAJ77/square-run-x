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
	glm::vec3 TextColor;

	float BrightnessThreshold;
	bool CursorEntered;
	uint32_t FontSize;
	
	std::function<void()> ExecutableFunction;
public:
	Button();
	Button(const Rect& TextureAtlas, const Rect& Destination, const Texture& BKGTexture, const Font& TextFont, 
		const std::string& Text, const std::function<void()>& Function, uint32_t FontSize = 48, 
		const glm::vec3& TextColor = glm::vec3(0.0f));
	~Button();

	void UpdateButton();
	void RenderButton(float Opacity = 1.0f, int TextOffset = 10) const;
};