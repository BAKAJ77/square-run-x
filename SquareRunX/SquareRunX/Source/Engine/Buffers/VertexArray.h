#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <vector>

typedef unsigned char ubyte;

struct VertexLayout
{
	uint32_t Index;
	int Size;
	uint32_t Type;
	
	bool Normalized;

	size_t Stride;
	size_t Offset;
	uint32_t Divisor;
};

class VertexArray
{
private:
	uint32_t ID;
	std::vector<VertexLayout> Layouts;
public:
	VertexArray();
	~VertexArray();

	void AttachBuffers(const VertexBufferPtr& VBO, const IndexBufferPtr& IBO = nullptr);

	void BindVertexArray() const;
	void UnbindVertexArray() const;
public:
	template<typename T> void PushLayout(uint32_t Index, int Size, size_t Stride, size_t Offset = 0, uint32_t Divisor = 0, 
		bool Normalized = false);
	template<> void PushLayout<float>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor,
		bool Normalized);
	template<> void PushLayout<uint32_t>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor,
		bool Normalized);
	template<> void PushLayout<ubyte>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor,
		bool Normalized);
public:
	const uint32_t& GetID() const;
};

#include "VertexArray.tpp"

typedef std::shared_ptr<VertexArray> VertexArrayPtr;

namespace Buffer
{
	static VertexArrayPtr GenerateVertexArray() { return std::make_shared<VertexArray>(); }
}