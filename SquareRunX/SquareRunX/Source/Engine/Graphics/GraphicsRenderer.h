#pragma once
#include "Engine/Graphics/ShaderManager.h"
#include "Engine/Buffers/VertexArray.h"
#include "Engine/Buffers/FrameBuffer.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/OrthogonalCamera.h"
#include "Engine/Graphics/FontLoaderTTF.h"

#include <glm/glm.hpp>
#include <vector>

struct Rect
{
	int x = 0, y = 0, w = 0, h = 0;
};

struct QuadRender
{
	const Texture* DiffuseMap;
	const Rect TextureAtlas; // AKA 'Source'

	std::vector<glm::mat4> ModelMatrices;
	bool ScreenSpace = false, Emissive = false;
};

struct TextRender
{
	const Font* CurrentFont;

	std::string String;
	glm::mat4 ModelMatrix;
	glm::vec4 Color;
};

struct BatchedData
{
	std::vector<float> VertexData;
	std::vector<uint32_t> IndexData;
};

class GraphicsRenderer
{
	friend class EngineCore;
	friend class PostProcessing;
private:
	mutable std::vector<QuadRender> QuadStack;
	mutable std::vector<TextRender> TextStack;
	glm::vec3 ClearColor;

	// Shader programs
	const ShaderProgram* QuadShader;
	const ShaderProgram* TextShader;

	// Quad Buffers
	VertexBufferPtr QuadVbo;
	IndexBufferPtr QuadIbo;
	VertexArrayPtr QuadVao;
private:
	GraphicsRenderer();
	~GraphicsRenderer();

	void InitRenderer();

	void BindShaderProgram(const ShaderProgram& Shader) const;
	BatchedData GenerateBatchTextData(const std::string& Text, const Font& CurrentFont) const;
public:
	void SetClearColor(const glm::vec3& Color);

	void RenderText(const glm::vec2& Pos, const uint32_t& FontSize, const std::string& Text, const Font& CurrentFont, 
		const glm::vec4& Color = { 0.0f, 0.0f, 0.0f, 1.0f }) const;

	/*
	* NOTE: 'Source' is the dimensions defining which part of the texture is sampled and
	* 'Destination' defines the screen position and size of the textured quad to be rendered.
	*/
	void RenderQuad(const Rect& Source, const Rect& Destination, const Texture& QuadTexture, 
		float RotationAngle = 0.0f, bool ScreenSpace = false, bool Emissive = false) const;

	void RefreshScreen(const FrameBufferPtr& RenderingFBO = nullptr) const;
	void FlushStack(const OrthoCamera& Camera, const FrameBufferPtr& RenderingFBO = nullptr, int ResolutionWidth = 2560,
		int ResolutionHeight = 1440) const;
public:
	static GraphicsRenderer& GetSingleton();
};

static bool operator==(const Rect& First, const Rect& Second)
{
	return First.x == Second.x && First.y == Second.y && First.w == Second.w && First.h == Second.h;
}