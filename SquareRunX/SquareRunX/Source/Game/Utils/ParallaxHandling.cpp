#include "ParallaxHandler.h"

ParallaxBKG::ParallaxBKG() :
	ScaleFactor(1.0)
{}

ParallaxBKG::ParallaxBKG(const std::vector<const Texture*>& Layers, double ScaleFactor) :
	Layers(Layers), ScaleFactor(ScaleFactor)
{
	this->GenerateScene(); // Generate enough layer loops to fill the screen
}

ParallaxBKG::~ParallaxBKG() {}

void ParallaxBKG::UpdateParallaxState(const std::vector<glm::dvec2>& Velocities, const OrthoCamera& SceneCamera,
	const double& DeltaTime)
{
	// Move the layers in respect to their set velocities
	for (uint32_t LayerIndex = 0; LayerIndex < this->Layers.size(); LayerIndex++)
	{
		const auto& LAYER_VELOCITY = Velocities[LayerIndex] * DeltaTime;
		auto& LayerPositions = this->LoopingPositions[LayerIndex];

		for (auto& Position : LayerPositions)
			Position += LAYER_VELOCITY;
	}

	// Update the visible layer positions to give an infinite parallax scrolling look
	uint32_t LayerIndex = 0;
	for (auto& LayerPositions : this->LoopingPositions)
	{
		for (auto& Position : LayerPositions)
		{
			glm::dvec2 LayerSize = { this->Layers[LayerIndex]->GetWidth() * this->ScaleFactor,
				this->Layers[LayerIndex]->GetHeight() * this->ScaleFactor };

			if (Position.x + LayerSize.x < 0.0)
			{
				double FurtherestTilePos = 0.0;
				for (const auto& OtherPosition : LayerPositions)
				{
					double LayerWidth = this->Layers[LayerIndex]->GetWidth() * this->ScaleFactor;
					if (OtherPosition.x + LayerWidth > FurtherestTilePos)
						FurtherestTilePos = OtherPosition.x + LayerWidth;
				}

				Position.x = FurtherestTilePos;
			}
			else if (Position.x > SceneCamera.GetViewSize().x)
			{
				double NearestTilePos = 0.0;
				for (const auto& OtherPosition : LayerPositions)
				{
					double LayerWidth = this->Layers[LayerIndex]->GetWidth() * this->ScaleFactor;
					if (OtherPosition.x - LayerWidth < NearestTilePos)
						NearestTilePos = OtherPosition.x - LayerWidth;
				}

				Position.x = NearestTilePos;
			}
		}

		LayerIndex++;
	}
}

void ParallaxBKG::RenderParallax() const
{
	for (uint32_t LayerIndex = 0; LayerIndex < this->Layers.size(); LayerIndex++)
	{
		for (const auto& Position : this->LoopingPositions[LayerIndex])
		{
			// Calculate source rect and destination rect
			Rect SourceRect = { 0, 0, this->Layers[LayerIndex]->GetWidth(), this->Layers[LayerIndex]->GetHeight() };
			Rect DestinationRect = { Position.x, Position.y, this->Layers[LayerIndex]->GetWidth() * this->ScaleFactor,
				this->Layers[LayerIndex]->GetHeight() * this->ScaleFactor };

			// Render the layer
			GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->Layers[LayerIndex], 0.0, true);
		}
	}
}

inline void ParallaxBKG::GenerateScene()
{
	for (const auto& Layer : this->Layers) // Loop through each layer
	{
		std::vector<glm::dvec2> Positions;
		for (double XPos = -(Layer->GetWidth() * this->ScaleFactor); XPos <= 1920.0;
			XPos += (Layer->GetWidth() * this->ScaleFactor))
		{
			Positions.emplace_back(glm::dvec2(XPos, 0.0));
		}

		this->LoopingPositions.emplace_back(Positions);
	}
}