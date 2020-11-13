#include "TileObject.h"

Tile::Tile() {}

Tile::Tile(const glm::ivec2& AtlasOffset, const glm::dvec2& Position) :
	TextureAtlas({ AtlasOffset.x, AtlasOffset.y, TileInfo::UNIFORM_SIZE, TileInfo::UNIFORM_SIZE }),
	Destination({ Position.x, Position.y, (double)TileInfo::UNIFORM_SIZE, (double)TileInfo::UNIFORM_SIZE })
{}
