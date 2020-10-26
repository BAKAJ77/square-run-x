#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"
#include <functional>
#include <algorithm>

namespace TransitionHandling
{
	static constexpr float TRANSITION_SPEED = 4000.0f;
	static constexpr float OPACITY_CHANGE_RATE = 0.1f;
	static constexpr float OPACITY_TIME_STEP = 0.01f;

	static void UpdateTransition(bool& TransitionComplete, bool& PreStageComplete, bool& StageOneComplete, bool& EndOfState,
		Rect& TransitionDest1, Rect& TransitionDest2, float& OpacityMultiplier, const OrthoCamera& SceneCamera, 
		const float& DeltaTime, const std::function<void()> PopStateFunc)
	{
		static float PrevTime = 0.0f;

		if (EndOfState)
		{
			if (!PreStageComplete)
			{
				if (WindowFrame::GetSingleton().GetTick() - PrevTime > OPACITY_TIME_STEP)
				{
					OpacityMultiplier = std::max(OpacityMultiplier - OPACITY_CHANGE_RATE, 0.0f);
					PrevTime = WindowFrame::GetSingleton().GetTick();

					if (OpacityMultiplier <= 0.0f)
						PreStageComplete = true;
				}
			}
			else if (!StageOneComplete)
			{
				if (TransitionDest2.x >= SceneCamera.GetViewSize().x)
					StageOneComplete = true;
				else
					TransitionDest2.x += (int)(TRANSITION_SPEED * DeltaTime);
			}
			else
			{
				if (TransitionDest1.y <= -TransitionDest1.h)
					PopStateFunc();
				else
					TransitionDest1.y -= (int)(TRANSITION_SPEED * DeltaTime);
			}
		}
		else
		{
			if (!PreStageComplete)
			{
				if (TransitionDest1.y + 100 >= 0)
					PreStageComplete = true;
				else
					TransitionDest1.y += (int)(TRANSITION_SPEED * DeltaTime);
			}
			else if(!StageOneComplete)
			{
				if ((TransitionDest2.x + TransitionDest2.w) - 200 <= SceneCamera.GetViewSize().x)
					StageOneComplete = true;
				else
					TransitionDest2.x -= (int)(TRANSITION_SPEED * DeltaTime);
			}
			else if (!TransitionComplete)
			{
				if (WindowFrame::GetSingleton().GetTick() - PrevTime > OPACITY_TIME_STEP)
				{
					OpacityMultiplier = std::min(OpacityMultiplier + OPACITY_CHANGE_RATE, 1.0f);
					PrevTime = WindowFrame::GetSingleton().GetTick();

					if (OpacityMultiplier >= 1.0f)
						TransitionComplete = true;
				}
			}
		}
	}
}