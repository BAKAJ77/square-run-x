#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"
#include <vector>

struct Animation
{
	std::string Name;
	std::vector<Rect> Frames;

	float TimePerFrame = 100.0f; // NOTE: In milliseconds
	bool Loop = true;
};

class AnimatedSprite
{
private:
	const Texture* SpriteSheetTex;
	
	std::vector<Animation> Animations;
	uint32_t CurrentAnimationIndex;
	mutable uint32_t CurrentFrameIndex;
	mutable double PreviousTime;

	bool Paused;
public:
	AnimatedSprite();
	AnimatedSprite(const Texture* SpriteSheet, bool Paused = false);
	~AnimatedSprite();

	void SetAnimationPaused(bool Paused);
	void SetCurrentAnimation(const std::string& Name);
	void SetCurrentFrameIndex(uint32_t Index);
	void AddAnimation(const std::string& Name, uint32_t NumFrames, const Rect& OriginFrame, float TimePerFrame = 100.0f, 
		uint32_t OffsetX = 0, bool Loop = true);

	void RenderAnimation(const Rect& Destination, float RotationAngle = 0.0f, float Brightness = 1.0, 
		float Opacity = 1.0f) const;
public:
	const std::vector<Animation>& GetAnimations() const;
};