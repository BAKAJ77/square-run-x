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
public:
	double x, y, w, h;
public:
	Rect() :
		x(0.0), y(0.0), w(0.0), h(0.0)
	{}

	Rect(int x, int y, int w, int h)
	{
		this->x = static_cast<double>(x);
		this->y = static_cast<double>(y);
		this->w = static_cast<double>(w);
		this->h = static_cast<double>(h);
	}

	Rect(double x, double y, double w, double h) :
		x(x), y(y), w(w), h(h)
	{}
};

struct QuadRender
{
	const Texture* DiffuseMap = nullptr;
	const Rect TextureAtlas; // AKA 'Source'

	glm::vec4 Color; // Used if not using textured quad

	std::vector<glm::mat4> ModelMatrices;
	bool ScreenSpace = false, Emissive = false;
	float BrightnessThreshold = 1.0f, OpacityMultiplier = 1.0f;
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
		const glm::vec4& Color = { 0.0f, 0.0f, 0.0f, 1.0f }, bool RaiseText = false) const;

	/*
	* NOTE: 'Source' is the dimensions defining which part of the texture is sampled and
	* 'Destination' defines the screen position and size of the textured quad to be rendered.
	* 
	* NOTE: Lower layers are drawn further top.
	*/
	void RenderQuad(const Rect& Source, const Rect& Destination, const Texture& QuadTexture, 
		double RotationAngle = 0.0, bool ScreenSpace = false, bool Emissive = false, float BrightnessThreshold = 1.0f,
		float OpacityMultiplier = 1.0f) const;

	void RenderFillQuad(const Rect& Destination, const glm::vec4& QuadColor, double RotationAngle = 0.0, 
		bool ScreenSpace = false, bool Emissive = false) const;

	void RefreshScreen(const FrameBufferPtr& RenderingFBO = nullptr) const;
	void FlushStack(const OrthoCamera& Camera, const FrameBufferPtr& RenderingFBO = nullptr, int ResolutionWidth = 2560,
		int ResolutionHeight = 1440) const;
public:
	static GraphicsRenderer& GetSingleton();
	glm::vec2 GetTextSize(const std::string& Text, const Font& CurrentFont, uint32_t FontSize) const;
};

static bool operator==(const Rect& First, const Rect& Second)
{
	return First.x == Second.x && First.y == Second.y && First.w == Second.w && First.h == Second.h;
}