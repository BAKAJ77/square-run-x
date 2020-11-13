#pragma once
#include "Engine/Buffers/TextureBuffer.h"
#include <unordered_map>
#include <string>

struct Texture
{
	friend class TextureManager;
	friend class GraphicsRenderer;
	friend bool operator==(const Texture& First, const Texture& Second);
private:
	std::string FilePath;
	Texture2DPtr TextureBuffer;
public:
	Texture() {}
	Texture(const std::string& FilePath, const Texture2DPtr& TextureBuffer) :
		FilePath(FilePath), TextureBuffer(TextureBuffer)
	{}

	const int GetWidth() const { return static_cast<int>(this->TextureBuffer->GetWidth()); }
	const int GetHeight() const { return static_cast<int>(this->TextureBuffer->GetHeight()); }
};

class TextureManager
{
	friend class EngineCore;
private:
	mutable std::unordered_map<std::string, Texture> LoadedTextures;
private:
	TextureManager();
	~TextureManager();

	void InitManager();
	void DestroyManager();
public:
	const Texture* LoadTexture(const std::string& Key, const std::string& FilePath, bool Srgb = false, bool FlipOnLoad = true);
	void DeleteTexture(const std::string& Key);
public:
	static TextureManager& GetSingleton();
};

static bool operator==(const Texture& First, const Texture& Second) { return First.FilePath == Second.FilePath; }