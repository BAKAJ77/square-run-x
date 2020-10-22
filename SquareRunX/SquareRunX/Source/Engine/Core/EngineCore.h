#pragma once
#include "Engine/Graphics/OrthogonalCamera.h"

class EngineCore
{
private:
	OrthoCamera Camera;
private:
	void InitSingletons();

	void MainLoop();
	void UpdateTick(const float& DeltaTime);
	void RenderTick() const;
public:
	EngineCore();
	~EngineCore();
};