#pragma once
#include <memory>

class IndexBuffer
{
private:
	uint32_t ID;
public:
	IndexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum);
	~IndexBuffer();

	void UpdateBufferData(const void* Data, uint32_t BufferOffset, uint32_t DataSize);

	void BindBuffer() const;
	void UnbindBuffer() const;
public:
	const uint32_t& GetID() const;
};

typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

namespace Buffer
{
	extern IndexBufferPtr GenerateIndexBuffer(const void* Data, uint32_t AllocSize, uint32_t UsageEnum);
}