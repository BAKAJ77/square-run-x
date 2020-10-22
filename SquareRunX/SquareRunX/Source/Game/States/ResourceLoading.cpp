#include "ResourceLoading.h"
#include "MainMenu.h"

#include <thread>
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;

namespace Threading
{
    std::mutex Mutex; // Ensure safe access of variables shared by multiple threads
}

void ResourceLoading::LoadGameFonts()
{
    // Load the fonts required by the game
    this->GameFonts[0] = { "Arial-Rounded",
        FontLoaderTTF::GetSingleton().LoadFont("ARIAL_ROUNDED", "GameFiles/Fonts/ARLRDBD.TTF") };
}

void ResourceLoading::LoadGameTextures()
{
    // Load the textures required by the game
    this->GameTextures[0] = { "Background-1",
        TextureManager::GetSingleton().LoadTexture("BKG_1", "GameFiles/Textures/BKG_1.png") };
    this->GameTextures[1] = { "Loading-Wheel",
        TextureManager::GetSingleton().LoadTexture("LOADING_WHEEL", "GameFiles/Textures/LOADING_WHEEL.png") };
    this->GameTextures[2] = { "Tile-Sheet",
        TextureManager::GetSingleton().LoadTexture("TILE_SHEET", "GameFiles/Textures/TILE_SHEET.png") };
}

void ResourceLoading::LoadGameAudio()
{
    // Load the music and sound effects required by the game
}

void ResourceLoading::LoadGameLevelMaps()
{
    std::this_thread::sleep_for(5s); // Pause the loading thread for a bit

    // Load the playable levels required by the game

    // Lock access to variable from other threads temporarily
    Threading::Mutex.lock();
    this->FinishedLoading = true;
    Threading::Mutex.unlock();
}

void ResourceLoading::InitState()
{
    this->LoadGameFonts();
    this->LoadGameTextures();
    this->LoadGameAudio();

    // Start loading the playable levels of the game
    std::thread LevelLoadingThread(&ResourceLoading::LoadGameLevelMaps, this);
    LevelLoadingThread.detach();
}

void ResourceLoading::DestroyState() {}

void ResourceLoading::UpdateTick(const float& DeltaTime)
{
    // If finished loading then smoothly reduce opacity of this state rendering
    if (this->FinishedLoading)
        this->HandleTransitions(TransitionType::HIDE);

    // Once completely transparent, switch to next game state which is the Main Menu
    if (PostProcessing::GetSingleton().GetOpacity() == 0.0f)
        this->SwitchState(MainMenu::GetGameState());

    this->LoadingWheelRotation += 425.0f * DeltaTime; // Update rotation position of the loading wheel
}

void ResourceLoading::RenderFrame() const
{
    // Clear the screen and fill with dark grey color
    GraphicsRenderer::GetSingleton().SetClearColor(glm::vec3(0.1f));

    // Render text and loading wheel to screen
    Rect SourceRect, DestinationRect;
    SourceRect = { 0, 0, 800, 800 };
    DestinationRect = { (int)(this->SceneCamera.GetViewSize().x / 2), (int)(this->SceneCamera.GetViewSize().y / 2) - 400, 
        60, 60 };

    GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->GetTexture("Loading-Wheel"), 
        this->LoadingWheelRotation);
}

ResourceLoading* ResourceLoading::GetGameState()
{
    static ResourceLoading State;
    return &State;
}

const Font* ResourceLoading::GetFont(const std::string& FontName) const
{
    for (const auto& Obj : this->GameFonts)
    {
        if (Obj.FontName == FontName)
            return Obj.Data;
    }

    return nullptr;
}

const Texture* ResourceLoading::GetTexture(const std::string& TextureName) const
{
    for (const auto& Obj : this->GameTextures)
    {
        if (Obj.TextureName == TextureName)
            return Obj.Data;
    }

    return nullptr;
}
