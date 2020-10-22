#include "TextureBuffer.h"
#include <glad/glad.h>

Texture2D::Texture2D(int MipmapLevel, int InternalFormat, size_t Width, size_t Height, uint32_t Format, uint32_t Type,
	const void* PixelData, bool GenerateMipmaps) :
	Width(Width), Height(Height), Target(GL_TEXTURE_2D)
{
	glGenTextures(1, &this->ID);
	glBindTexture(this->Target, this->ID);
	glTexImage2D(this->Target, MipmapLevel, InternalFormat, static_cast<GLsizei>(Width), static_cast<GLsizei>(Height), 
		0, Format, Type, PixelData);

	if (GenerateMipmaps)
		glGenerateMipmap(this->Target);

	// Set the default wrapping and filtering mode for the texture
	this->SetWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	this->SetFilteringMode(GL_LINEAR, GL_LINEAR);

	glBindTexture(this->Target, 0);
}

Texture2D::Texture2D(uint32_t InternalFormat, size_t Width, size_t Height, uint32_t Samples, bool FixedSamplePoints) :
	Width(Width), Height(Height), Target(GL_TEXTURE_2D_MULTISAMPLE)
{
	glGenTextures(1, &this->ID);
	glBindTexture(this->Target, this->ID);
	glTexImage2DMultisample(this->Target, Samples, InternalFormat, static_cast<GLsizei>(Width), static_cast<GLsizei>(Height),
		FixedSamplePoints);

	// Set the default wrapping and filtering mode for the texture
	this->SetWrappingMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	this->SetFilteringMode(GL_LINEAR, GL_LINEAR);

	glBindTexture(this->Target, 0);
}

Texture2D::~Texture2D() { glDeleteTextures(1, &this->ID); }

void Texture2D::SetWrappingMode(uint32_t SAxis, uint32_t TAxis) const
{
	glBindTexture(this->Target, this->ID);
	glTexParameteri(this->Target, GL_TEXTURE_WRAP_S, SAxis);
	glTexParameteri(this->Target, GL_TEXTURE_WRAP_T, TAxis);
	glBindTexture(this->Target, 0);
}

void Texture2D::SetFilteringMode(uint32_t Min, uint32_t Mag) const
{
	glBindTexture(this->Target, this->ID);
	glTexParameteri(this->Target, GL_TEXTURE_MIN_FILTER, Min);
	glTexParameteri(this->Target, GL_TEXTURE_MAG_FILTER, Mag);
	glBindTexture(this->Target, 0);
}

void Texture2D::BindTexture() const
{
	glBindTexture(this->Target, this->ID);
}

void Texture2D::BindTexture(uint32_t Index) const
{
	glActiveTexture(GL_TEXTURE0 + Index);
	glBindTexture(this->Target, this->ID);
}

void Texture2D::UnbindTexture() const { glBindTexture(this->Target, 0); }

const uint32_t& Texture2D::GetID() const { return this->ID; }

const uint32_t& Texture2D::GetTarget() const { return this->Target; }

const size_t& Texture2D::GetWidth() const { return this->Width; }

const size_t& Texture2D::GetHeight() const { return this->Height; }

namespace Buffer
{
	Texture2DPtr GenerateTexture2D(int MipmapLevel, int InternalFormat, size_t Width, size_t Height, uint32_t Format,
		uint32_t Type, const void* PixelData, bool GenerateMipmaps)
	{
		return std::make_shared<Texture2D>(MipmapLevel, InternalFormat, Width, Height, Format, Type, PixelData, GenerateMipmaps);
	}

	Texture2DPtr GenerateTexture2D(uint32_t InternalFormat, size_t Width, size_t Height, uint32_t Samples, 
		bool FixedSamplePoints)
	{
		return std::make_shared<Texture2D>(InternalFormat, Width, Height, Samples, FixedSamplePoints);
	}
}