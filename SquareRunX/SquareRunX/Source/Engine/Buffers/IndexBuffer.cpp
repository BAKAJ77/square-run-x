#include "IndexBuffer.h"
#include <glad/glad.h>

IndexBuffer::IndexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum)
{
	glGenBuffers(1, &this->ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, AllocSize, Data, UsageEnum);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &this->ID); }

void IndexBuffer::UpdateBufferData(const void* Data, uint32_t BufferOffset, uint32_t DataSize)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, BufferOffset, DataSize, Data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::BindBuffer() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID); }

void IndexBuffer::UnbindBuffer() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

const uint32_t& IndexBuffer::GetID() const { return this->ID; }

namespace Buffer
{
	IndexBufferPtr GenerateIndexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum)
	{
		return std::make_shared<IndexBuffer>(Data, AllocSize, UsageEnum);
	}
}