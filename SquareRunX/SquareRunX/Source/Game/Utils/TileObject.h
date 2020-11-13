#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"

namespace TileInfo
{
	static constexpr int UNIFORM_SIZE = 80;
}

class Tile
{
public:
	Rect TextureAtlas, Destination;
public:
	Tile();
	Tile(const glm::ivec2& AtlasOffset, const glm::dvec2& Position);
};