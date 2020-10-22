#pragma once
#include <memory>

class VertexBuffer
{
private:
	uint32_t ID;
public:
	VertexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum);
	~VertexBuffer();

	void UpdateBufferData(const void* Data, uint32_t BufferOffset, uint32_t DataSize);

	void BindBuffer() const;
	void UnbindBuffer() const;
public:
	const uint32_t& GetID() const;
};

typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

namespace Buffer
{
	extern VertexBufferPtr GenerateVertexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum);
}