#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgram
{
	friend class GraphicsRenderer;
	friend class ShaderManager;
public:
	uint32_t ID;
	std::string VertexPath, FragmentPath;

	mutable std::unordered_map<std::string, uint32_t> UniformCache;
private:
	uint32_t QueryUniformLocation(const std::string& UniformName) const;
public:
	ShaderProgram() :
		ID(0)
	{}

	ShaderProgram(uint32_t ID, const std::string& VertexPath, const std::string& FragmentPath) :
		ID(ID), VertexPath(VertexPath), FragmentPath(FragmentPath)
	{}

	template<typename T> void SetUniform(const std::string& Uniform, T value) const;
	template<> void SetUniform<int>(const std::string& Uniform, int value) const;
	template<> void SetUniform<bool>(const std::string& Uniform, bool value) const;
	template<> void SetUniform<float>(const std::string& Uniform, float value) const;

	template<typename T> void SetUniformEx(const std::string& Uniform, const T& values) const;
	template<> void SetUniformEx<glm::vec2>(const std::string& Uniform, const glm::vec2& values) const;
	template<> void SetUniformEx<glm::vec3>(const std::string& Uniform, const glm::vec3& values) const;
	template<> void SetUniformEx<glm::vec4>(const std::string& Uniform, const glm::vec4& values) const;
	template<> void SetUniformEx<glm::mat3>(const std::string& Uniform, const glm::mat3& values) const;
	template<> void SetUniformEx<glm::mat4>(const std::string& Uniform, const glm::mat4& values) const;
};

#include "ShaderManager.tpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ProcessType
{
	COMPILATION,
	LINKING
};

class ShaderManager
{
	friend class EngineCore;
private:
	mutable std::unordered_map<std::string, ShaderProgram> LoadedShaders;
private:
	ShaderManager();
	~ShaderManager();

	void InitManager();
	void DestroyManager();

	void VerifyShaderLoadProcess(const uint32_t& ID, ProcessType Process) const;
public:
	const ShaderProgram* LoadShaders(const std::string& Key, const std::string& VertexPath, const std::string& FragmentPath);
	void DeleteShaders(const std::string& Key);
public:
	static ShaderManager& GetSingleton();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////