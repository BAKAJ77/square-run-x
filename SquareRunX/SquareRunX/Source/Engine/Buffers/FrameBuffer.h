#pragma once
#include "RenderBuffer.h"
#include "TextureBuffer.h"

class FrameBuffer
{
private:
	uint32_t ID;
	bool UsingColorAttachment;
public:
	FrameBuffer(bool UsingColorAttachment);
	~FrameBuffer();

	void AttachTexture2D(uint32_t Attachment, const Texture2DPtr& Buffer) const;
	void AttachRenderBuffer(uint32_t Attachement, const RenderBufferPtr& Buffer) const;

	void BindBuffer() const;
	void UnbindBuffer() const;
public:
	const uint32_t& GetID() const;
};

typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

namespace Buffer
{
	extern FrameBufferPtr GenerateFrameBuffer(bool UsingColorAttachment = true);
}