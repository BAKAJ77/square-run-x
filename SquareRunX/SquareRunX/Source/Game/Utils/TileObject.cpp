#include "TileObject.h"

Tile::Tile() {}

Tile::Tile(const glm::ivec2& AtlasOffset, const glm::ivec2& Position) :
	TextureAtlas({ AtlasOffset.x, AtlasOffset.y, TileInfo::UNIFORM_ATLAS_SIZE, TileInfo::UNIFORM_ATLAS_SIZE }),
	Destination({ Position.x, Position.y, TileInfo::UNIFORM_INGAME_SIZE, TileInfo::UNIFORM_INGAME_SIZE })
{}
