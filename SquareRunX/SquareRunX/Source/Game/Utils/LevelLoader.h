#pragma once
#include "TileObject.h"

#include "Engine/Core/AudioPlayer.h"
#include "Engine/Graphics/OrthogonalCamera.h"
#include "Engine/External/rapidxml/rapidxml.hpp"
#include "Game/Utils/PhysicsHandling.h"
#include "Game/Utils/ParallaxHandler.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Texture;

struct TileAtlas
{
	int GidIndex = 0;
	Rect TextureAtlas;
};

struct Tileset
{
	const Texture* TileSheetTex = nullptr;

	int FirstGidIndex = 0;
	std::vector<TileAtlas> TileAtlases;
};

struct SceneLayer
{
	std::string Name;
	std::vector<Tile> Tiles;
};

class LevelMap
{
private:
	// The loaded level metadata
	std::string Name, ThemeAudioPath;
	uint32_t Width, Height;
	int LevelIndex, ActIndex;

	// The parsed level contents
	ParallaxBKG MapBackground;

	Tileset TileSheetObj;
	std::vector<SceneLayer> Layers;
	std::vector<ColliderBox> SafeColliders, DamagingColliders;
	
	glm::dvec2 FirstLevelSpawnPoint, LevelFinishPoint;
	std::vector<glm::dvec2> PlayerCheckpoints;
private:
	void RetrieveLevelMetadata(const rapidxml::xml_node<>* RootNode);
	void RetrieveParallaxBKG(const rapidxml::xml_node<>* RootNode);
	void RetrieveTilesetData(const rapidxml::xml_node<>* RootNode);
	void RetrieveLevelTiles(const rapidxml::xml_node<>* RootNode);

	void LoadSpawnPoints(const rapidxml::xml_node<>* RootNode);
	void LoadTileColliders(const rapidxml::xml_node<>* RootNode);
public:
	LevelMap(const std::string& FilePath, const std::string& ThemeAudioPath);
	~LevelMap();

	void RenderTiles(const OrthoCamera& SceneCamera, const double& Brightness) const;
public:
	const std::string& GetLevelName() const;
	const int& GetLevelIndex() const;
	const int& GetActIndex() const;

	const std::string& GetThemeAudioPath() const;

	const uint32_t& GetWidthPixels() const;
	const uint32_t& GetHeightPixels() const;

	const glm::dvec2& GetFirstLevelSpawnPoint() const;
	const glm::dvec2& GetLevelFinishPoint() const;

	const std::vector<glm::dvec2>& GetPlayerCheckpoints() const;
	const std::vector<ColliderBox>& GetSafeColliders() const;
	const std::vector<ColliderBox>& GetDamagingColliders() const;

	ParallaxBKG& GetBackground();
};