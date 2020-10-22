#include "RenderBuffer.h"
#include <glad/glad.h>

RenderBuffer::RenderBuffer(uint32_t InternalFormat, int Width, int Height) :
	Width(Width), Height(Height), NumSamples(1)
{
	glGenRenderbuffers(1, &this->ID);
	glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
	glRenderbufferStorage(GL_RENDERBUFFER, InternalFormat, Width, Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::RenderBuffer(uint32_t InternalFormat, int Width, int Height, size_t NumSamples) :
	Width(Width), Height(Height), NumSamples(NumSamples)
{
	glGenRenderbuffers(1, &this->ID);
	glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<int>(NumSamples), InternalFormat, Width, Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::~RenderBuffer() { glDeleteRenderbuffers(1, &this->ID); }

void RenderBuffer::BindBuffer() const { glBindRenderbuffer(GL_RENDERBUFFER, this->ID); }

void RenderBuffer::UnbindBuffer() const { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

const uint32_t RenderBuffer::GetID() const { return this->ID; }

namespace Buffer
{
	RenderBufferPtr GenerateRenderBuffer(uint32_t InternalFormat, int Width, int Height)
	{
		return std::make_shared<RenderBuffer>(InternalFormat, Width, Height);
	}

	RenderBufferPtr GenerateRenderBuffer(uint32_t InternalFormat, int Width, int Height, size_t NumSamples)
	{
		return std::make_shared<RenderBuffer>(InternalFormat, Width, Height, NumSamples);
	}
}