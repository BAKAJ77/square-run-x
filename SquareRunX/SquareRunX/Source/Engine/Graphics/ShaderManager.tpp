#include "ShaderManager.h"
#include <glad/glad.h>

template<typename T> 
void ShaderProgram::SetUniform(const std::string& Uniform, T value) const { static_assert(false); }

template<>
void ShaderProgram::SetUniform<int>(const std::string& Uniform, int value) const 
{ 
	glUniform1i(this->QueryUniformLocation(Uniform), value);
}

template<>
void ShaderProgram::SetUniform<float>(const std::string& Uniform, float value) const
{
	glUniform1f(this->QueryUniformLocation(Uniform), value);
}

template<>
void ShaderProgram::SetUniform<double>(const std::string& Uniform, double value) const
{
	glUniform1f(this->QueryUniformLocation(Uniform), static_cast<float>(value));
}


template<>
void ShaderProgram::SetUniform<bool>(const std::string& Uniform, bool value) const
{
	glUniform1i(this->QueryUniformLocation(Uniform), static_cast<int>(value));
}

template<typename T> 
void ShaderProgram::SetUniformEx(const std::string& Uniform, const T& values) const { static_assert(false); }

template<>
void ShaderProgram::SetUniformEx<glm::vec2>(const std::string& Uniform, const glm::vec2& values) const 
{ 
	glUniform2fv(this->QueryUniformLocation(Uniform), 1, &values[0]);
}

template<>
void ShaderProgram::SetUniformEx<glm::vec3>(const std::string& Uniform, const glm::vec3& values) const
{
	glUniform3fv(this->QueryUniformLocation(Uniform), 1, &values[0]);
}

template<>
void ShaderProgram::SetUniformEx<glm::vec4>(const std::string& Uniform, const glm::vec4& values) const
{
	glUniform4fv(this->QueryUniformLocation(Uniform), 1, &values[0]);
}

template<>
void ShaderProgram::SetUniformEx<glm::mat3>(const std::string& Uniform, const glm::mat3& values) const
{
	glUniformMatrix3fv(this->QueryUniformLocation(Uniform), 1, false, &values[0][0]);
}

template<>
void ShaderProgram::SetUniformEx<glm::mat4>(const std::string& Uniform, const glm::mat4& values) const
{
	glUniformMatrix4fv(this->QueryUniformLocation(Uniform), 1, false, &values[0][0]);
}

template<>
void ShaderProgram::SetUniformEx<glm::dmat4>(const std::string& Uniform, const glm::dmat4& values) const
{
	glm::mat4 MatrixFloat = values;
	glUniformMatrix4fv(this->QueryUniformLocation(Uniform), 1, false, &MatrixFloat[0][0]);
}