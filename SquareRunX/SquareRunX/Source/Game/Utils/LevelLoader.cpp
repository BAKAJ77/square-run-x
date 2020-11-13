#include "LevelLoader.h"

#include "Engine/Graphics/GraphicsRenderer.h"
#include "Engine/Filesystem/FileHandler.h"
#include "Engine/Debug/LoggingManager.h"

#include <algorithm>

LevelMap::LevelMap(const std::string& FilePath) :
    LevelIndex(0), ActIndex(0), Width(0), Height(0)
{
    // Load the entire contents of the level file into string
    FileHandler::GetSingleton().OpenFile("LEVEL_FILE", FilePath.c_str(), FileFlags::READ_MODE);
    std::string XMLFileContents = FileHandler::GetSingleton().ReadData("LEVEL_FILE").str();
    FileHandler::GetSingleton().CloseFile("LEVEL_FILE");

    // Now parse the level XML file contents loaded
    rapidxml::xml_document LevelDocument;
    LevelDocument.parse<0>(&XMLFileContents[0]);

    // Now perform multiple function stages, to extract all required level data
    const rapidxml::xml_node<>* ROOT_NODE = LevelDocument.first_node("map");
    if (ROOT_NODE)
    {
        this->RetrieveLevelMetadata(ROOT_NODE);
        this->RetrieveParallaxBKG(ROOT_NODE);
        this->RetrieveTilesetData(ROOT_NODE);
        this->RetrieveLevelTiles(ROOT_NODE);

        this->LoadSpawnPoints(ROOT_NODE);
        this->LoadTileColliders(ROOT_NODE);
    }
    else
        LogManager::GetSingleton().OutputLog("Failed to access root node of level XML file \"" + this->Name + "\".", 
            LogLevel::FATAL);
}

LevelMap::~LevelMap() {}

void LevelMap::RenderTiles(const OrthoCamera& SceneCamera) const
{
    Rect CameraViewBox = { (int)SceneCamera.GetPosition().x, (int)SceneCamera.GetPosition().y,
        (int)SceneCamera.GetViewSize().x, (int)SceneCamera.GetViewSize().y };

    // Render the map's background
    this->MapBackground.RenderParallax();

    // Render the tiles in the level scene
    for (const auto& Layer : this->Layers)
    {
        for (const auto& TileObj : Layer.Tiles)
        {
            // Render only the tiles which are visible, this saves lots of performance
            if (TileObj.Destination.x + TileObj.Destination.w >= CameraViewBox.x &&
                TileObj.Destination.x <= CameraViewBox.x + CameraViewBox.w &&
                TileObj.Destination.y + TileObj.Destination.h >= CameraViewBox.y &&
                TileObj.Destination.y <= CameraViewBox.y + CameraViewBox.h)
            {
                GraphicsRenderer::GetSingleton().RenderQuad(TileObj.TextureAtlas, TileObj.Destination,
                    *this->TileSheetObj.TileSheetTex);
            }
        }
    }

}

void LevelMap::RetrieveLevelMetadata(const rapidxml::xml_node<>* RootNode)
{
    // Retrieve metadata from attributes of 'map' node which is the root node
    for (rapidxml::xml_node<>* ChildNode = RootNode->first_node("properties")->first_node("property"); ChildNode != nullptr;
        ChildNode = ChildNode->next_sibling("property"))
    {
        const std::string PROPERTY_NAME = ChildNode->first_attribute("name")->value();
        
        if (PROPERTY_NAME == "levelname")
        {
            this->Name = ChildNode->first_attribute("value")->value();
            std::transform(this->Name.begin(), this->Name.end(), this->Name.begin(), ::toupper);
        }
        else if (PROPERTY_NAME == "levelindex")
            this->LevelIndex = std::stoi(ChildNode->first_attribute("value")->value());
        else if (PROPERTY_NAME == "actindex")
            this->ActIndex = std::stoi(ChildNode->first_attribute("value")->value());
    }

    // Retrieve the dimensions (width and height) of the level
    this->Width = std::stoi(RootNode->first_attribute("width")->value()) * TileInfo::UNIFORM_SIZE;
    this->Height = std::stoi(RootNode->first_attribute("height")->value()) * TileInfo::UNIFORM_SIZE;
}

void LevelMap::RetrieveTilesetData(const rapidxml::xml_node<>* RootNode)
{
    const rapidxml::xml_node<>* CHILD_TILESET_NODE = RootNode->first_node("tileset");
    if (CHILD_TILESET_NODE)
    {
        // Retrieve the tilesheet texture
        const std::string TILESHEET_PATH = CHILD_TILESET_NODE->first_node("image")->first_attribute("source")->value();
        this->TileSheetObj.TileSheetTex = TextureManager::GetSingleton().LoadTexture("TILE_SHEET", TILESHEET_PATH);
        
        // Retrieve details for indexing individual tiles in tilesheet
        this->TileSheetObj.FirstGidIndex = std::stoi(CHILD_TILESET_NODE->first_attribute("firstgid")->value());
        const int NUM_TILE_COLUMNS = std::stoi(CHILD_TILESET_NODE->first_attribute("columns")->value());
        const int TOTAL_TILES = std::stoi(CHILD_TILESET_NODE->first_attribute("tilecount")->value());

        this->TileSheetObj.TileAtlases.reserve(TOTAL_TILES);

        // Generate the tile atlases for each tile in the tileset
        int OffsetX = 0, OffsetY = 0;
        for (int i = 1; i < TOTAL_TILES + 1; i++)
        {
            this->TileSheetObj.TileAtlases.push_back(
                { i, { OffsetX, OffsetY, TileInfo::UNIFORM_SIZE, TileInfo::UNIFORM_SIZE } });

            if (((i + 1) % NUM_TILE_COLUMNS == 1 || NUM_TILE_COLUMNS == 1) && (i + 1) > 1)
            {
                OffsetX = 0;
                OffsetY += TileInfo::UNIFORM_SIZE;
            }
            else
                OffsetX += TileInfo::UNIFORM_SIZE;
        }
    }
    else
        LogManager::GetSingleton().OutputLog("Failed to access child node 'tileset' in the level XML file named \"" +
            this->Name + "\".", LogLevel::FATAL);
}

void LevelMap::RetrieveLevelTiles(const rapidxml::xml_node<>* RootNode)
{
    // Loop through each layer node in the root node branch
    for (rapidxml::xml_node<>* LayerNode = RootNode->first_node("layer"); LayerNode != nullptr;
        LayerNode = LayerNode->next_sibling("layer"))
    {
        SceneLayer LoadedLayer;
        LoadedLayer.Name = LayerNode->first_attribute("name")->value();

        std::string LevelTileData = LayerNode->first_node("data")->value();

        // Remove end-of-line characters
        for (auto It = LevelTileData.begin(); It != LevelTileData.end(); It++)
        {
            if (*It == '\n')
            {
                LevelTileData.erase(It);
                It = LevelTileData.begin();
            }
        }

        // Loop through tile data and extract the tile texture and position
        glm::ivec2 TileDestination = { 0, this->Height - TileInfo::UNIFORM_SIZE };
        const int LEVEL_WIDTH = this->Width / TileInfo::UNIFORM_SIZE;
        
        uint32_t TileIndex = 1, LastCommaLocation = 0;
        for (uint32_t i = 0; i < LevelTileData.size(); i++)
        { 
            if (LevelTileData[i] == ',' || i == LevelTileData.size() - 1)
            {
                const int TILESHEET_INDEX = std::stoi(LevelTileData.substr(LastCommaLocation, 
                    std::max((int)(i - LastCommaLocation), 1)));
                glm::ivec2 TileAtlasPos;

                // Figure out the position of the tile
                if (TileIndex > static_cast<uint32_t>(LEVEL_WIDTH))
                {
                    TileDestination.x = 0;
                    TileDestination.y -= TileInfo::UNIFORM_SIZE;

                    TileIndex = 1;
                }
                else
                    TileDestination.x = (TileIndex - 1) * TileInfo::UNIFORM_SIZE;

                // Get the tile texture atlas from tilesheet
                if (TILESHEET_INDEX >= this->TileSheetObj.FirstGidIndex)
                {
                    for (const auto& TileAtlas : this->TileSheetObj.TileAtlases)
                    {
                        if (TILESHEET_INDEX == TileAtlas.GidIndex)
                        {
                            TileAtlasPos = { TileAtlas.TextureAtlas.x, TileAtlas.TextureAtlas.y };
                            break;
                        }
                    }

                    // Push loaded tile to layer tile vector array
                    LoadedLayer.Tiles.emplace_back(Tile(TileAtlasPos, TileDestination));
                }

                TileIndex++;
                LastCommaLocation = i + 1;
            }
        }

        this->Layers.emplace_back(LoadedLayer);
    }
}

void LevelMap::LoadSpawnPoints(const rapidxml::xml_node<>* RootNode)
{
    // Loop through the object groups to find the spawn points object group
    for (rapidxml::xml_node<>* ObjectGroup = RootNode->first_node("objectgroup"); ObjectGroup != nullptr;
        ObjectGroup = ObjectGroup->next_sibling("objectgroup"))
    {
        const std::string OBJECTGROUP_NAME = ObjectGroup->first_attribute("name")->value();

        // Get the group of spawn point objects
        if (OBJECTGROUP_NAME == "Spawn Points")
        {
            for (rapidxml::xml_node<>* Object = ObjectGroup->first_node("object"); Object != nullptr;
                Object = Object->next_sibling("object")) // Loop through the spawn point objects
            {
                if (std::string(Object->first_attribute("name")->value()) == "CHECKPOINT")
                {
                    this->PlayerCheckpoints.push_back({ std::stod(Object->first_attribute("x")->value()),
                       this->Height - std::stod(Object->first_attribute("y")->value()) });
                }
                else if (std::string(Object->first_attribute("name")->value()) == "ENEMY")
                {
                    // Get enemy location (TODO)
                }
                else if (std::string(Object->first_attribute("name")->value()) == "POWERUP")
                {
                    // Get powerup location (TODO)
                }
            }
        }
    }
}

void LevelMap::LoadTileColliders(const rapidxml::xml_node<>* RootNode)
{
    // Loop through the object groups to find the tile colliders object group
    for (rapidxml::xml_node<>* ObjectGroup = RootNode->first_node("objectgroup"); ObjectGroup != nullptr;
        ObjectGroup = ObjectGroup->next_sibling("objectgroup"))
    {
        const std::string OBJECTGROUP_NAME = ObjectGroup->first_attribute("name")->value();

        // Get the group of tile collider objects
        if (OBJECTGROUP_NAME == "Colliders")
        {
            for (rapidxml::xml_node<>* Object = ObjectGroup->first_node("object"); Object != nullptr;
                Object = Object->next_sibling("object")) // Loop through the tile collider objects
            {
                if (std::string(Object->first_attribute("name")->value()) == "SAFE")
                {
                    ColliderBox BoundingBox;
                    BoundingBox.x = std::stod(Object->first_attribute("x")->value());
                    BoundingBox.w = std::stod(Object->first_attribute("width")->value());
                    BoundingBox.h = std::stod(Object->first_attribute("height")->value());

                    BoundingBox.y = this->Height - (std::stod(Object->first_attribute("y")->value()) + BoundingBox.h);
                    this->SafeColliders.emplace_back(BoundingBox);
                }
                else if (std::string(Object->first_attribute("name")->value()) == "DAMAGING")
                {
                    ColliderBox BoundingBox;
                    BoundingBox.x = std::stod(Object->first_attribute("x")->value());
                    BoundingBox.w = std::stod(Object->first_attribute("width")->value());
                    BoundingBox.h = std::stod(Object->first_attribute("height")->value());

                    BoundingBox.y = this->Height - (std::stod(Object->first_attribute("y")->value()) + BoundingBox.h);
                    this->DamagingColliders.emplace_back(BoundingBox);
                }
            }
        }
    }
}

void LevelMap::RetrieveParallaxBKG(const rapidxml::xml_node<>* RootNode)
{
    std::string ParallaxKey, ParallaxPath;
    int NumParallaxLayers = 0;
    double ParallaxScaleFactor = 0.0;

    // Retrieve parallax bkg data from attributes of 'map' node which is the root node
    for (rapidxml::xml_node<>* ChildNode = RootNode->first_node("properties")->first_node("property"); ChildNode != nullptr;
        ChildNode = ChildNode->next_sibling("property"))
    {
        const std::string PROPERTY_NAME = ChildNode->first_attribute("name")->value();

        if (PROPERTY_NAME == "parallaxkey")
            ParallaxKey = ChildNode->first_attribute("value")->value();
        else if (PROPERTY_NAME == "parallaxpath")
            ParallaxPath = ChildNode->first_attribute("value")->value();
        else if (PROPERTY_NAME == "parallaxlayers")
            NumParallaxLayers = std::stoi(ChildNode->first_attribute("value")->value());
        else if (PROPERTY_NAME == "parallaxscale")
            ParallaxScaleFactor = std::stod(ChildNode->first_attribute("value")->value());
    }

    // Load the parallax background
    std::vector<const Texture*> LayerTextures;
    LayerTextures.reserve(NumParallaxLayers);

    for (int Index = 0; Index < NumParallaxLayers; Index++)
    {
        // Generate texture key and path
        const std::string TEXTURE_KEY = ParallaxKey + "_" + std::to_string(Index + 1);
        const std::string TEXTURE_PATH = ParallaxPath + "/" + std::to_string(Index + 1) + ".png";

        LayerTextures.emplace_back(TextureManager::GetSingleton().LoadTexture(TEXTURE_KEY, TEXTURE_PATH));
    }

    this->MapBackground = ParallaxBKG(LayerTextures, ParallaxScaleFactor);
}

const std::string& LevelMap::GetLevelName() const { return this->Name; }
const int& LevelMap::GetLevelIndex() const { return this->LevelIndex; }
const int& LevelMap::GetActIndex() const { return this->ActIndex; }

const uint32_t& LevelMap::GetWidthPixels() const { return this->Width; }
const uint32_t& LevelMap::GetHeightPixels() const { return this->Height; }

const std::vector<glm::dvec2>& LevelMap::GetPlayerCheckpoints() const { return this->PlayerCheckpoints; }

const std::vector<ColliderBox>& LevelMap::GetSafeColliders() const { return this->SafeColliders; }
const std::vector<ColliderBox>& LevelMap::GetDamagingColliders() const { return this->DamagingColliders; }

ParallaxBKG& LevelMap::GetBackground() { return this->MapBackground; }