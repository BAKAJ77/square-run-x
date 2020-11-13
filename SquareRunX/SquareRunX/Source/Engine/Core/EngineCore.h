#pragma once
#include "Engine/Debug/PerformanceCounter.h"

class EngineCore
{
private:
	mutable PerformanceCounter PerfCounter;
	bool DisplayPerformanceCounter;
private:
	void InitSingletons();
	void LoadConfigSettings();

	void MainLoop();
	void UpdateTick(const double& DeltaTime);
	void RenderTick() const;
public:
	EngineCore();
	~EngineCore();
};