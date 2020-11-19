#include "PerformanceCounter.h"
#include "Engine/Graphics/FontLoaderTTF.h"
#include "Engine/Core/WindowFrame.h"

PerformanceCounter::PerformanceCounter() :
	FramesRendered(0), RecordedFPSCount(0), UpdateTicks(0), RecordedUpdateTicks(0), ConsolasBoldFont(nullptr),
	Enabled(false)
{}

PerformanceCounter::~PerformanceCounter() {}

void PerformanceCounter::InitializeCounter()
{
	this->ConsolasBoldFont = FontLoaderTTF::GetSingleton().LoadFont("CONSOLAS_BOLD", "GameFiles/Fonts/CONSOLAZ.TTF");
}

void PerformanceCounter::SwitchEnabledState()
{
	this->Enabled = !this->Enabled;
}

void PerformanceCounter::IncrementRenderCounter()
{
	this->FramesRendered++;

	// Record the number of frames rendered each second
	static double PreviousTime = WindowFrame::GetSingleton().GetTick();
	if (WindowFrame::GetSingleton().GetTick() - PreviousTime >= 1000.0)
	{
		this->RecordedFPSCount = this->FramesRendered;
		this->FramesRendered = 0;

		PreviousTime = WindowFrame::GetSingleton().GetTick();
	}
}

void PerformanceCounter::IncrementUpdateCounter()
{
	this->UpdateTicks++;

	// Record the number of frames rendered each second
	static double PreviousTime = WindowFrame::GetSingleton().GetTick();
	if (WindowFrame::GetSingleton().GetTick() - PreviousTime >= 1000.0)
	{
		this->RecordedUpdateTicks = this->UpdateTicks;
		this->UpdateTicks = 0;

		PreviousTime = WindowFrame::GetSingleton().GetTick();
	}
}

void PerformanceCounter::RenderCounter() const
{
	if (this->Enabled)
	{
		constexpr int TEXT_OFFSET = -20, FONT_SIZE = 50;

		// Render the recorded performance counters
		const std::string RECORDED_STR = "FPS: " + std::to_string(this->RecordedFPSCount) + " | Update Ticks: " +
			std::to_string(this->RecordedUpdateTicks);
		const int TEXT_WIDTH =
			static_cast<int>(GraphicsRenderer::GetSingleton().GetTextSize(RECORDED_STR, *this->ConsolasBoldFont, FONT_SIZE).x);

		GraphicsRenderer::GetSingleton().RenderText({ (1920 - TEXT_WIDTH) - TEXT_OFFSET, 1020 }, FONT_SIZE, RECORDED_STR,
			*this->ConsolasBoldFont, { 0.0f, 1.0f, 0.0f, 1.0f }, true);
	}
}
