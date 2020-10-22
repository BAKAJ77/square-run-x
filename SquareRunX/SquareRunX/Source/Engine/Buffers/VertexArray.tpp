#pragma once
#include "VertexArray.h"
#include <glad/glad.h>

template<typename T> 
void VertexArray::PushLayout(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor, 
	bool Normalized) { static_assert(false); }

template<>
void VertexArray::PushLayout<float>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor, 
	bool Normalized) 
{
	this->Layouts.push_back({ Index, Size, GL_FLOAT, Normalized, Stride, Offset, Divisor });
}

template<>
void VertexArray::PushLayout<uint32_t>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor, 
	bool Normalized)
{
	this->Layouts.push_back({ Index, Size, GL_UNSIGNED_INT, Normalized, Stride, Offset, Divisor });
}

template<>
void VertexArray::PushLayout<ubyte>(uint32_t Index, int Size, size_t Stride, size_t Offset, uint32_t Divisor,
	bool Normalized)
{
	this->Layouts.push_back({ Index, Size, GL_UNSIGNED_BYTE, Normalized, Stride, Offset, Divisor });
}