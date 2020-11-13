#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"
#include <glm/glm.hpp>
#include <vector>

class ParallaxBKG
{
private:
	std::vector<const Texture*> Layers;
	std::vector<std::vector<glm::dvec2>> LoopingPositions;

	double ScaleFactor;
private:
	void GenerateScene();
public:
	ParallaxBKG();
	ParallaxBKG(const std::vector<const Texture*>& Layers, double ScaleFactor = 1.0);
	~ParallaxBKG();

	void UpdateParallaxState(const std::vector<glm::dvec2>& Velocities, const OrthoCamera& SceneCamera,
		const double& DeltaTime);
	void RenderParallax() const;
};