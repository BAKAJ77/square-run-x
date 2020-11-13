#include "AnimationSprite.h"
#include "Engine/Debug/LoggingManager.h"
#include "Engine/Core/WindowFrame.h"

AnimatedSprite::AnimatedSprite() :
    CurrentAnimationIndex(0), CurrentFrameIndex(0), PreviousTime(WindowFrame::GetSingleton().GetTick()), SpriteSheetTex(nullptr)
{}

AnimatedSprite::AnimatedSprite(const Texture* SpriteSheet) :
    SpriteSheetTex(SpriteSheet), CurrentAnimationIndex(0), CurrentFrameIndex(0), 
    PreviousTime(WindowFrame::GetSingleton().GetTick())
{}

AnimatedSprite::~AnimatedSprite() {}

void AnimatedSprite::SetCurrentAnimation(const std::string& Name)
{
    for (uint32_t Index = 0; Index < this->Animations.size(); Index++)
    {
        if (this->Animations[Index].Name == Name)
        {
            this->CurrentAnimationIndex = Index;
            this->CurrentFrameIndex = 0;

            this->PreviousTime = WindowFrame::GetSingleton().GetTick();
            break;
        }
    }
}

void AnimatedSprite::SetCurrentFrameIndex(uint32_t Index)
{
    if (!this->Animations.empty())
    {
        if (Index < this->Animations[this->CurrentAnimationIndex].Frames.size())
        {
            this->CurrentFrameIndex = Index;
            this->PreviousTime = WindowFrame::GetSingleton().GetTick();
        }
    }
}

void AnimatedSprite::AddAnimation(const std::string& Name, uint32_t NumFrames, const Rect& OriginFrame, float TimePerFrame, 
    uint32_t OffsetX, bool Loop)
{
    // Make sure the name hasn't been taken already
    for (const auto& ExistingAnim : this->Animations)
    {
        if (ExistingAnim.Name == Name)
        {
            LogManager::GetSingleton().OutputLog("Animation name (\"" + Name + "\") has already been taken, use another.",
                LogLevel::WARNING);
            return;
        }
    }

    Animation NewAnimation;
    NewAnimation.Name = Name;
    NewAnimation.TimePerFrame = TimePerFrame;
    NewAnimation.Loop = Loop;

    // Extract the individual frames of the animation
    std::vector<Rect> ExtractedFrames;
    for (uint32_t i = 0; i < NumFrames; i++)
    {
        Rect FrameAtlas;
        FrameAtlas.x = static_cast<double>(OriginFrame.x) + (static_cast<double>(i) * OriginFrame.w) +
            static_cast<double>(OffsetX);
        FrameAtlas.y = static_cast<double>(OriginFrame.y);
        FrameAtlas.w = static_cast<double>(OriginFrame.w);
        FrameAtlas.h = static_cast<double>(OriginFrame.h);

        ExtractedFrames.emplace_back(FrameAtlas);
    }

    NewAnimation.Frames = ExtractedFrames;
    this->Animations.emplace_back(NewAnimation);
}

void AnimatedSprite::RenderAnimation(const Rect& Destination, float RotationAngle, float Opacity) const
{
    if (this->SpriteSheetTex) // Make sure sprite sheet texture is valid
    {
        const Animation& CURRENT_ANIMATION = this->Animations[this->CurrentAnimationIndex];
        
        // Reset frame index 
        if (this->CurrentFrameIndex >= CURRENT_ANIMATION.Frames.size())
        {
            if(CURRENT_ANIMATION.Loop)
                this->CurrentFrameIndex = 0;
            else
                this->CurrentFrameIndex = static_cast<uint32_t>(CURRENT_ANIMATION.Frames.size() - 1);
        }

        GraphicsRenderer::GetSingleton().RenderQuad(CURRENT_ANIMATION.Frames[this->CurrentFrameIndex], Destination, 
            *this->SpriteSheetTex, RotationAngle, false, false, 1.0f, Opacity);

        // Increment current frame index if frame time is exceeded
        if (WindowFrame::GetSingleton().GetTick() - this->PreviousTime >= CURRENT_ANIMATION.TimePerFrame)
        {
            this->CurrentFrameIndex++;
            this->PreviousTime = WindowFrame::GetSingleton().GetTick();
        }
    }
}

const std::vector<Animation>& AnimatedSprite::GetAnimations() const { return this->Animations; }
