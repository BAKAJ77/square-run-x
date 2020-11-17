#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"

class PerformanceCounter
{
private:
	const Font* ConsolasBoldFont;
	int FramesRendered, RecordedFPSCount;
	int UpdateTicks, RecordedUpdateTicks;

	bool Enabled;
public:
	PerformanceCounter();
	~PerformanceCounter();

	void InitializeCounter();
	void SwitchEnabledState(); 

	void IncrementUpdateCounter();
	void IncrementRenderCounter();
	void RenderCounter() const;
};