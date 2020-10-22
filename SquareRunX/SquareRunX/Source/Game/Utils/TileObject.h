#pragma once
#include "Engine/Graphics/GraphicsRenderer.h"

namespace TileInfo
{
	static constexpr int UNIFORM_ATLAS_SIZE = 120;
	static constexpr int UNIFORM_INGAME_SIZE = 80;
}

class Tile
{
public:
	Rect TextureAtlas, Destination;
public:
	Tile();
	Tile(const glm::ivec2& AtlasOffset, const glm::ivec2& Position);
};