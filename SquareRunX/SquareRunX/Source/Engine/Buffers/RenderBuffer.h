#pragma once
#include <memory>

class RenderBuffer
{
private:
	uint32_t ID;
	
	int Width, Height;
	size_t NumSamples;
public:
	RenderBuffer(uint32_t InternalFormat, int Width, int Height);
	RenderBuffer(uint32_t InternalFormat, int Width, int Height, size_t NumSamples);
	~RenderBuffer();

	void BindBuffer() const;
	void UnbindBuffer() const;
public:
	const uint32_t GetID() const;
};

typedef std::shared_ptr<RenderBuffer> RenderBufferPtr;

namespace Buffer
{
	extern RenderBufferPtr GenerateRenderBuffer(uint32_t InternalFormat, int Width, int Height);
	extern RenderBufferPtr GenerateRenderBuffer(uint32_t InternalFormat, int Width, int Height, size_t NumSamples);
}