#include "FrameBuffer.h"
#include "Engine/Debug/LoggingManager.h"

#include <glad/glad.h>

FrameBuffer::FrameBuffer(bool UsingColorAttachment) :
	UsingColorAttachment(UsingColorAttachment)
{
	glGenFramebuffers(1, &this->ID);
	if (!UsingColorAttachment)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

FrameBuffer::~FrameBuffer() { glDeleteFramebuffers(1, &this->ID); }

void FrameBuffer::AttachTexture2D(uint32_t Attachment, const Texture2DPtr& Buffer) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment, Buffer->GetTarget(), Buffer->GetID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AttachRenderBuffer(uint32_t Attachment, const RenderBufferPtr& Buffer) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, Attachment, GL_RENDERBUFFER, Buffer->GetID());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindBuffer() const 
{ 
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID); 

	// Make sure the framebuffer is setup completely
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LogManager::GetSingleton().OutputLog("The framebuffer (ID = " + std::to_string(this->ID) + ") is not complete",
			LogLevel::WARNING);
}

void FrameBuffer::UnbindBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

const uint32_t& FrameBuffer::GetID() const { return this->ID; }

namespace Buffer
{
	FrameBufferPtr GenerateFrameBuffer(bool UsingColorAttachment)
	{
		return std::make_shared<FrameBuffer>(UsingColorAttachment);
	}
}