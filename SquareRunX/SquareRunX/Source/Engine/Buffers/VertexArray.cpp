#include "VertexArray.h"
#include <glad/glad.h>

VertexArray::VertexArray() { glGenVertexArrays(1, &this->ID); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &this->ID); }

void VertexArray::AttachBuffers(const VertexBufferPtr& VBO, const IndexBufferPtr& IBO)
{
	// Bind the VAO and the buffers
	glBindVertexArray(this->ID);
	VBO->BindBuffer();
	if (IBO)
		IBO->BindBuffer();

	// Define the vertex attribs for OpenGL to interpret for use in shaders
	for (const auto& Layout : this->Layouts)
	{
		glEnableVertexAttribArray(Layout.Index);
		glVertexAttribPointer(Layout.Index, Layout.Size, Layout.Type, Layout.Normalized, static_cast<GLsizei>(Layout.Stride), 
			reinterpret_cast<void*>(Layout.Offset));
		glVertexAttribDivisor(Layout.Index, Layout.Divisor);
	}

	// Unbind the VAO and the buffers
	glBindVertexArray(0);
	VBO->UnbindBuffer();
	if (IBO)
		IBO->UnbindBuffer();

	this->Layouts.clear();
}

void VertexArray::BindVertexArray() const { glBindVertexArray(this->ID); }

void VertexArray::UnbindVertexArray() const { glBindVertexArray(0); }

const uint32_t& VertexArray::GetID() const { return this->ID; }