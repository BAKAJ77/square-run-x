#pragma once
#include "Engine/Buffers/TextureBuffer.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <string>

struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

struct Character
{
	glm::vec2 Bearing, Size;
	uint32_t AdvanceX = 0;
	size_t TextureOffsetX = 0;
};

struct Font
{
	std::string FilePath;

	mutable std::unordered_map<unsigned char, Character> Metrics;
	Texture2DPtr FontBitmap;
};

class FontLoaderTTF
{
	friend class EngineCore;
private:
	FT_Library FreeTypeLib;
	mutable std::unordered_map<std::string, Font> LoadedFonts;

	uint32_t FontQuality;
private:
	FontLoaderTTF();
	~FontLoaderTTF();

	void InitLoader();
	void DestroyLoader();
public:
	const Font* LoadFont(const std::string& Key, const std::string& Path);
	void DeleteFont(const std::string& Key);
public:
	static FontLoaderTTF& GetSingleton();
	const uint32_t& GetFontQuality() const;
};