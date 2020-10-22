#pragma once
#include <memory>

class Texture2D
{
private:
	uint32_t ID, Target;
	size_t Width, Height;
public:
	Texture2D(int MipmapLevel, int InternalFormat, size_t Width, size_t Height, uint32_t Format, uint32_t Type, 
		const void* PixelData, bool GenerateMipmaps);
	Texture2D(uint32_t InternalFormat, size_t Width, size_t Height, uint32_t Samples, bool FixedSamplePoints);
	~Texture2D();

	void SetWrappingMode(uint32_t SAxis, uint32_t TAxis) const;
	void SetFilteringMode(uint32_t Min, uint32_t Mag) const;

	void BindTexture() const;
	void BindTexture(uint32_t Index) const;
	void UnbindTexture() const;
public:
	const uint32_t& GetID() const;
	const uint32_t& GetTarget() const;

	const size_t& GetWidth() const;
	const size_t& GetHeight() const;
};

typedef std::shared_ptr<Texture2D> Texture2DPtr;

namespace Buffer
{
	extern Texture2DPtr GenerateTexture2D(int MipmapLevel, int InternalFormat, size_t Width, size_t Height, uint32_t Format, 
		uint32_t Type, const void* PixelData, bool GenerateMipmaps = false);
	extern Texture2DPtr GenerateTexture2D(uint32_t InternalFormat, size_t Width, size_t Height, uint32_t Samples, 
		bool FixedSamplePoints = true);
}