#include "TextureManager.h"
#include "Engine/Filesystem/CFGConfigLoader.h"
#include "Engine/Debug/LoggingManager.h"
#include <glad/glad.h>
#include <stb_image.h>

TextureManager::TextureManager() {}

TextureManager::~TextureManager() {}

void TextureManager::InitManager()
{
	// Load the texture config settings from file
	CFGConfigLoader::GetSingleton().OpenFile("Performance");
	const int RESERVE_LIMIT = 
		CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("TextureReserveLimit"));
	CFGConfigLoader::GetSingleton().CloseFile();

	this->LoadedTextures.reserve(RESERVE_LIMIT);
}

void TextureManager::DestroyManager()
{
	this->LoadedTextures.clear();
}

const Texture* TextureManager::LoadTexture(const std::string& Key, const std::string& FilePath, bool Srgb, bool FlipOnLoad)
{
	// Prevent the same texture from being loaded more than once
	for (const auto& TextureObj : this->LoadedTextures)
	{
		if (TextureObj.first == Key || TextureObj.second.FilePath == FilePath)
			return &TextureObj.second;
	}

	stbi_set_flip_vertically_on_load(FlipOnLoad);

	// Load the texture data from file
	int Width, Height, Channels;
	uint32_t InternalFormat, Format;
	unsigned char* PixelData = stbi_load(FilePath.c_str(), &Width, &Height, &Channels, 0);
	if (PixelData)
	{
		if (Channels > 3)
		{
			Format = GL_RGBA;
			Srgb ? InternalFormat = GL_SRGB_ALPHA : InternalFormat = GL_RGBA;
		}
		else
		{
			Format = GL_RGB;
			Srgb ? InternalFormat = GL_SRGB : InternalFormat = GL_RGB;
		}

		this->LoadedTextures[Key] = { FilePath,
			Buffer::GenerateTexture2D(0, InternalFormat, Width, Height, Format, GL_UNSIGNED_BYTE, PixelData) };
	}
	else
		LogManager::GetSingleton().OutputLog("Failed to load texture at path: \"" + FilePath + "\"",
			LogLevel::FATAL);

	stbi_image_free(PixelData);
	return &this->LoadedTextures[Key];
}

void TextureManager::DeleteTexture(const std::string& Key) { this->LoadedTextures.erase(Key); }

TextureManager& TextureManager::GetSingleton()
{
	static TextureManager Singleton;
	return Singleton;
}