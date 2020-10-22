#include "VertexBuffer.h"
#include <glad/glad.h>

VertexBuffer::VertexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum)
{
	glGenBuffers(1, &this->ID);
	glBindBuffer(GL_ARRAY_BUFFER, this->ID);
	glBufferData(GL_ARRAY_BUFFER, AllocSize, Data, UsageEnum);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &this->ID); }

void VertexBuffer::UpdateBufferData(const void* Data, uint32_t BufferOffset, uint32_t DataSize)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->ID);
	glBufferSubData(GL_ARRAY_BUFFER, BufferOffset, DataSize, Data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::BindBuffer() const { glBindBuffer(GL_ARRAY_BUFFER, this->ID); }

void VertexBuffer::UnbindBuffer() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

const uint32_t& VertexBuffer::GetID() const { return this->ID; }

namespace Buffer
{
	VertexBufferPtr GenerateVertexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum)
	{
		return std::make_shared<VertexBuffer>(Data, AllocSize, UsageEnum);
	}
}