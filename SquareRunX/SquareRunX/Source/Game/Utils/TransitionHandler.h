#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"
#include <functional>
#include <algorithm>

namespace TransitionHandling
{
	static constexpr float TRANSITION_SPEED = 4.0f;
	static constexpr float OPACITY_CHANGE_RATE = 0.005f;

	static void UpdateTransition(bool& TransitionComplete, bool& PreStageComplete, bool& StageOneComplete, bool& EndOfState,
		Rect& TransitionDest1, Rect& TransitionDest2, float& OpacityMultiplier, const OrthoCamera& SceneCamera, 
		const double& DeltaTime, const std::function<void()> PopStateFunc)
	{
		if (EndOfState)
		{
			if (!PreStageComplete)
			{
				OpacityMultiplier = static_cast<float>(std::max(OpacityMultiplier - (OPACITY_CHANGE_RATE * DeltaTime), 0.0));

				if (OpacityMultiplier <= 0.0f)
					PreStageComplete = true;
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
				TransitionDest1.y += (int)(TRANSITION_SPEED * DeltaTime);
				if (TransitionDest1.y + 100 >= 0)
				{
					TransitionDest1.y = -100;
					PreStageComplete = true;
				}
			}
			else if(!StageOneComplete)
			{
				TransitionDest2.x -= (int)(TRANSITION_SPEED * DeltaTime);
				if ((TransitionDest2.x + TransitionDest2.w) - 200 <= SceneCamera.GetViewSize().x)
				{
					TransitionDest2.x = (int)SceneCamera.GetViewSize().x - (TransitionDest2.w - 175);
					StageOneComplete = true;
				}
			}
			else if (!TransitionComplete)
			{
				OpacityMultiplier = static_cast<float>(std::min(OpacityMultiplier + (OPACITY_CHANGE_RATE * DeltaTime), 1.0));
				
				if (OpacityMultiplier >= 1.0f)
					TransitionComplete = true;
			}
		}
	}
}