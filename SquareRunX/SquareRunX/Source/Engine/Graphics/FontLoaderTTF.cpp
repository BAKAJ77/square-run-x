#include "FontLoaderTTF.h"
#include "Engine/Debug/LoggingManager.h"
#include "Engine/Filesystem/CFGConfigLoader.h"

#include <algorithm>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FontLoaderTTF::FontLoaderTTF() :
	FreeTypeLib(nullptr), FontQuality(150)
{}

FontLoaderTTF::~FontLoaderTTF() {}

void FontLoaderTTF::InitLoader()
{
	// Initialize the Freetype library
	if (FT_Init_FreeType(&this->FreeTypeLib))
		LogManager::GetSingleton().OutputLog("Failed to initialize the FreeType library", LogLevel::FATAL);

	// Load the 'TextQuality' config from the config file
	CFGConfigLoader::GetSingleton().OpenFile("GraphicsQuality");
	this->FontQuality = static_cast<uint32_t>(CFGElement::ToDataValue<int>(
		*CFGConfigLoader::GetSingleton().QueryElement("TextQuality")));
	CFGConfigLoader::GetSingleton().CloseFile();

	// Clamp the font quality value loaded
	this->FontQuality = std::clamp(static_cast<int>(this->FontQuality), 30, 230);
}

void FontLoaderTTF::DestroyLoader()
{
	FT_Done_FreeType(this->FreeTypeLib);
	this->LoadedFonts.clear();
}

const Font* FontLoaderTTF::LoadFont(const std::string& Key, const std::string& Path)
{
	// Search and return pointer to font if already loaded
	for (const auto& FontData : this->LoadedFonts)
	{
		if (FontData.second.FilePath == Path || FontData.first == Key)
			return &FontData.second;
	}

	// Load the font face
	FT_Face FontFace;
	if (FT_New_Face(this->FreeTypeLib, Path.c_str(), 0, &FontFace))
		LogManager::GetSingleton().OutputLog("An error occured while loading the font at path: \"" + Path + "\"",
			LogLevel::FATAL);

	FT_Set_Pixel_Sizes(FontFace, 0, this->FontQuality); // Set the pixel size of the font

	// Get the metrics of each character from the font
	// Also get the combined widths of each character and the maximum height, this is needed for allocating the
	// bitmap texture.
	size_t BitmapWidth = 0, BitmapHeight = 0;
	std::unordered_map<unsigned char, Character> Metrics;

	for (unsigned char CurrentChar = 0; CurrentChar < 128; CurrentChar++)
	{
		if (FT_Load_Char(FontFace, CurrentChar, FT_LOAD_BITMAP_METRICS_ONLY))
			LogManager::GetSingleton().OutputLog("An error occured while loading bitmap metrics from the font at path: \"" +
				Path + "\"", LogLevel::FATAL);

		Character CharData;
		CharData.Bearing = { FontFace->glyph->bitmap_left, FontFace->glyph->bitmap_top };
		CharData.Size = { FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows };
		CharData.AdvanceX = FontFace->glyph->advance.x;
		CharData.TextureOffsetX = BitmapWidth;

		Metrics[CurrentChar] = CharData;

		// Update the bitmap width (and height if glyph is taller than previous tallest glyph)
		const size_t GLYPH_BITMAP_OFFSET = 20; // This to prevent glyph bitmaps from overlapping

		BitmapWidth += static_cast<size_t>(CharData.Size.x) + GLYPH_BITMAP_OFFSET;
		if (BitmapHeight < static_cast<size_t>(CharData.Size.y))
			BitmapHeight = static_cast<size_t>(CharData.Size.y);
	}

	// Allocate memory for the font bitmap texture
	Texture2DPtr BitmapTexture = Buffer::GenerateTexture2D(0, GL_RED, BitmapWidth, BitmapHeight, GL_RED, GL_UNSIGNED_BYTE,
		nullptr);

	// Lastly we fill the allocated memory with the glyph bitmap pixels
	BitmapTexture->BindTexture();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char CurrentChar = 0; CurrentChar < 128; CurrentChar++)
	{
		if (FT_Load_Char(FontFace, CurrentChar, FT_LOAD_RENDER))
			LogManager::GetSingleton().OutputLog("An error occured while loading bitmap pixels from the font at path: \"" +
				Path + "\"", LogLevel::FATAL);

		const Character& CharData = Metrics[CurrentChar];
		glTexSubImage2D(BitmapTexture->GetTarget(), 0, static_cast<GLint>(CharData.TextureOffsetX), 0,
			static_cast<GLsizei>(CharData.Size.x), static_cast<GLsizei>(CharData.Size.y), GL_RED, GL_UNSIGNED_BYTE,
			FontFace->glyph->bitmap.buffer);
	}

	BitmapTexture->UnbindTexture();
	BitmapTexture->SetWrappingMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

	// Add the font to the loaded font map
	this->LoadedFonts[Key] = { Path, Metrics, BitmapTexture };
	return &this->LoadedFonts[Key];
}

void FontLoaderTTF::DeleteFont(const std::string& Key)
{
	this->LoadedFonts.erase(Key);
}

FontLoaderTTF& FontLoaderTTF::GetSingleton()
{
	static FontLoaderTTF Singleton;
	return Singleton;
}

const uint32_t& FontLoaderTTF::GetFontQuality() const { return this->FontQuality; }