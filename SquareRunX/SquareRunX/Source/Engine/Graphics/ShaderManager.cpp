#include "ShaderManager.h"
#include "Engine/Filesystem/CFGConfigLoader.h"
#include "Engine/Debug/LoggingManager.h"

#include <sstream>
#include <fstream>
#include <glad/glad.h>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ShaderProgram::QueryUniformLocation(const std::string& UniformName) const
{
	// Check if uniform location is already loaded into the cache map
	auto Iterator = this->UniformCache.find(UniformName);
	if (Iterator != this->UniformCache.end())
		return Iterator->second;

	// It has not been found therefore we get the uniform location
	const uint32_t UNIFORM_LOC = glGetUniformLocation(this->ID, UniformName.c_str());
	this->UniformCache[UniformName] = UNIFORM_LOC;
	
	return UNIFORM_LOC;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {}

void ShaderManager::InitManager()
{
	CFGConfigLoader::GetSingleton().OpenFile("Performance");
	const int RESERVE_LIMIT =
		CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("ShaderReserveLimit"));
	CFGConfigLoader::GetSingleton().CloseFile();

	this->LoadedShaders.reserve(RESERVE_LIMIT);
}

void ShaderManager::DestroyManager()
{
	for (auto& Shaders : this->LoadedShaders)
		glDeleteProgram(Shaders.second.ID);
}

void ShaderManager::VerifyShaderLoadProcess(const uint32_t& ID, ProcessType Process) const
{
	int LogLength = 0;
	std::unique_ptr<char> LogBuffer;

	switch (Process)
	{
	case ProcessType::COMPILATION:
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &LogLength);
		if (LogLength > 0)
		{
			LogBuffer = std::unique_ptr<char>(new char[LogLength]);
			glGetShaderInfoLog(ID, LogLength, nullptr, LogBuffer.get());
		}
		break;
	case ProcessType::LINKING:
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &LogLength);
		if (LogLength > 0)
		{
			LogBuffer = std::unique_ptr<char>(new char[LogLength]);
			glGetProgramInfoLog(ID, LogLength, nullptr, LogBuffer.get());
		}
		break;
	}

	if (LogLength > 0)
		LogManager::GetSingleton().OutputLog(LogBuffer.get(), LogLevel::FATAL);
}

const ShaderProgram* ShaderManager::LoadShaders(const std::string& Key, const std::string& VertexPath, 
	const std::string& FragmentPath)
{
	// Check and return pointer to existing shader if identical
	for (const auto& Shader : this->LoadedShaders)
	{
		if ((Shader.second.VertexPath == VertexPath && Shader.second.FragmentPath == FragmentPath) ||
			Shader.first == Key)
			return &Shader.second;
	}

	std::stringstream VertexStream, FragmentStream;
	std::ifstream ShaderFile;

	// Open the shader files and load their contents
	// VERTEX SHADER LOADING
	ShaderFile.open(VertexPath);
	if (ShaderFile.fail())
		LogManager::GetSingleton().OutputLog("Failed to load vertex shader at path: \"" + VertexPath + "\"",
			LogLevel::FATAL);

	VertexStream << ShaderFile.rdbuf();
	const std::string VERTEX_STR = VertexStream.str();
	ShaderFile.close();

	// FRAGMENT SHADER LOADING
	ShaderFile.open(FragmentPath);
	if (ShaderFile.fail())
		LogManager::GetSingleton().OutputLog("Failed to load fragment shader at path: \"" + FragmentPath + "\"",
			LogLevel::FATAL);

	FragmentStream << ShaderFile.rdbuf();
	const std::string FRAGMENT_STR = FragmentStream.str();
	ShaderFile.close();

	// Now we generate the shader objects and compile the contents loaded
	const char* VERTEX_SOURCE = VERTEX_STR.c_str();
	const char* FRAGMENT_SOURCE = FRAGMENT_STR.c_str();

	// VERTEX SHADER COMPILATION
	uint32_t VertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexID, 1, &VERTEX_SOURCE, nullptr);
	glCompileShader(VertexID);

	this->VerifyShaderLoadProcess(VertexID, ProcessType::COMPILATION);

	// FRAGMENT SHADER COMPILATION
	uint32_t FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentID, 1, &FRAGMENT_SOURCE, nullptr);
	glCompileShader(FragmentID);

	this->VerifyShaderLoadProcess(FragmentID, ProcessType::COMPILATION);

	// We can delete the shader objects after we have attached and linked the them to the shader program
	const uint32_t SHADER_PROGRAM = glCreateProgram();
	glAttachShader(SHADER_PROGRAM, VertexID);
	glAttachShader(SHADER_PROGRAM, FragmentID);
	glLinkProgram(SHADER_PROGRAM);

	this->VerifyShaderLoadProcess(SHADER_PROGRAM, ProcessType::LINKING);
	glDeleteShader(VertexID);
	glDeleteShader(FragmentID);

	this->LoadedShaders[Key] = { SHADER_PROGRAM, VertexPath, FragmentPath };
	return &this->LoadedShaders[Key];
}

void ShaderManager::DeleteShaders(const std::string& Key)
{
	glDeleteProgram(this->LoadedShaders[Key].ID);
	this->LoadedShaders.erase(Key);
}

ShaderManager& ShaderManager::GetSingleton()
{
	static ShaderManager Singleton;
	return Singleton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////