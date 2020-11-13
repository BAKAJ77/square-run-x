#include "GraphicsRenderer.h"
#include "Engine/Filesystem/CFGConfigLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <algorithm>
#include <array>

GraphicsRenderer::GraphicsRenderer() :
	QuadShader(nullptr), TextShader(nullptr)
{}

GraphicsRenderer::~GraphicsRenderer() {}

void GraphicsRenderer::InitRenderer()
{
	// Load the required shaders
	this->QuadShader = ShaderManager::GetSingleton().LoadShaders("QUAD_SHADER", "GameFiles/Shaders/QuadShader.glsl.vsh",
		"GameFiles/Shaders/QuadShader.glsl.fsh");
	this->TextShader = ShaderManager::GetSingleton().LoadShaders("TEXT_SHADER", "GameFiles/Shaders/TextShader.glsl.vsh",
		"GameFiles/Shaders/TextShader.glsl.fsh");

	// Get the renderer configurations from file
	CFGConfigLoader::GetSingleton().OpenFile("Performance");
	const int TOTAL_RESERVE = 
		CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("RenderReserveLimit"));
	CFGConfigLoader::GetSingleton().CloseFile();

	this->QuadStack.reserve(std::max(TOTAL_RESERVE, 0));
	this->TextStack.reserve(std::max(TOTAL_RESERVE, 0));

	// Initialize the required regular quad buffers
	std::array<float, 16> QuadVertexData
	{
		 // Vertex Coords
		-0.5f, -0.5f,      
		 0.5f, -0.5f,     
		-0.5f,  0.5f,      
		 0.5f,  0.5f,

		 // UV Coords
		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 0.0f, 1.0f,
		 1.0f, 1.0f 
	};

	std::array<uint32_t, 6> QuadIndexData
	{
		0, 1, 2,
		2, 1, 3
	};

	this->QuadVbo = Buffer::GenerateVertexBuffer(QuadVertexData.data(), sizeof(QuadVertexData), GL_DYNAMIC_DRAW);
	this->QuadIbo = Buffer::GenerateIndexBuffer(QuadIndexData.data(), sizeof(QuadIndexData), GL_STATIC_DRAW);
	this->QuadVao = Buffer::GenerateVertexArray();

	this->QuadVao->PushLayout<float>(0, 2, 2 * sizeof(float)); // Vertex coordinates layout
	this->QuadVao->PushLayout<float>(1, 2, 2 * sizeof(float), 8 * sizeof(float)); // UV coordinates layout
	this->QuadVao->AttachBuffers(this->QuadVbo, this->QuadIbo);

	// Enable support for blending, this is used for handling semi/fully transparent colors
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

BatchedData GraphicsRenderer::GenerateBatchTextData(const std::string& Text, const Font& CurrentFont) const
{
	BatchedData Data;

	glm::vec2 OriginPosition;
	uint32_t IndexingOffset = 0;
	bool FirstCharacter = true;

	for (const char& Char : Text)
	{
		// Generate the vertex and UV coords
		const Character& CHAR_METRICS = CurrentFont.Metrics[Char];

		// VERTEX COORDS
		glm::vec2 TopLeftVertex;
		if (FirstCharacter)
			TopLeftVertex = { OriginPosition.x, OriginPosition.y + (CHAR_METRICS.Bearing.y - CHAR_METRICS.Size.y) };
		else
			TopLeftVertex = { OriginPosition.x + CHAR_METRICS.Bearing.x, 
			OriginPosition.y + (CHAR_METRICS.Bearing.y - CHAR_METRICS.Size.y) };

		glm::vec2 TopRightVertex = { TopLeftVertex.x + CHAR_METRICS.Size.x, TopLeftVertex.y };
		glm::vec2 BottomLeftVertex = { TopLeftVertex.x, TopLeftVertex.y + CHAR_METRICS.Size.y };
		glm::vec2 BottomRightVertex = { TopLeftVertex.x + CHAR_METRICS.Size.x,
			TopLeftVertex.y + CHAR_METRICS.Size.y };

		// UV COORDS
		glm::vec2 TopLeftUV = { CHAR_METRICS.TextureOffsetX, CHAR_METRICS.Size.y };
		glm::vec2 TopRightUV = { TopLeftUV.x + CHAR_METRICS.Size.x, TopLeftUV.y };
		glm::vec2 BottomLeftUV = { TopLeftUV.x, 0.0f };
		glm::vec2 BottomRightUV = { TopLeftUV.x + CHAR_METRICS.Size.x, 0.0f };

		// Normalize the UV coords into the range -1.0f to 1.0f
		TopLeftUV.x /= static_cast<float>(CurrentFont.FontBitmap->GetWidth());
		TopRightUV.x /= static_cast<float>(CurrentFont.FontBitmap->GetWidth());
		BottomLeftUV.x /= static_cast<float>(CurrentFont.FontBitmap->GetWidth());
		BottomRightUV.x /= static_cast<float>(CurrentFont.FontBitmap->GetWidth());
		
		TopLeftUV.y /= static_cast<float>(CurrentFont.FontBitmap->GetHeight());
		TopRightUV.y /= static_cast<float>(CurrentFont.FontBitmap->GetHeight());
		BottomLeftUV.y /= static_cast<float>(CurrentFont.FontBitmap->GetHeight());
		BottomRightUV.y /= static_cast<float>(CurrentFont.FontBitmap->GetHeight());

		// Create the vertex data array
		std::array<float, 16> VertexArray
		{
			TopLeftVertex.x, TopLeftVertex.y, TopLeftUV.x, TopLeftUV.y,
			TopRightVertex.x, TopRightVertex.y, TopRightUV.x, TopRightUV.y,
			BottomLeftVertex.x, BottomLeftVertex.y, BottomLeftUV.x, BottomLeftUV.y,
			BottomRightVertex.x, BottomRightVertex.y, BottomRightUV.x, BottomRightUV.y
		};

		Data.VertexData.insert(Data.VertexData.end(), VertexArray.begin(), VertexArray.end());

		// Create the index data array
		std::array<uint32_t, 6> IndexingArray
		{
			0 + IndexingOffset, 2 + IndexingOffset, 3 + IndexingOffset,
			0 + IndexingOffset, 3 + IndexingOffset, 1 + IndexingOffset
		};

		Data.IndexData.insert(Data.IndexData.end(), IndexingArray.begin(), IndexingArray.end());

		if (FirstCharacter)
		{
			OriginPosition.x += (CHAR_METRICS.AdvanceX >> 6) - CHAR_METRICS.Bearing.x;
			FirstCharacter = false;
		}
		else
			OriginPosition.x += (CHAR_METRICS.AdvanceX >> 6);

		IndexingOffset += 4;
	}

	return Data;
}

void GraphicsRenderer::BindShaderProgram(const ShaderProgram& Shader) const
{
	static uint32_t CurrentBoundShaderID = 0;
	if (Shader.ID != CurrentBoundShaderID)
	{
		CurrentBoundShaderID = Shader.ID;
		glUseProgram(CurrentBoundShaderID);
	}
}

void GraphicsRenderer::SetClearColor(const glm::vec3& Color) { this->ClearColor = Color; }

void GraphicsRenderer::RenderText(const glm::vec2& Pos, const uint32_t& FontSize, const std::string& Text, 
	const Font& CurrentFont, const glm::vec4& Color, bool RaiseText) const
{
	// Calculate the model matrix
	if (!Text.empty())
	{
		if (RaiseText)
		{
			glm::mat4 Model;
			Model = glm::translate(Model, glm::vec3(Pos - 5.0f, 0.0f));
			Model = glm::scale(Model, glm::vec3(
				glm::vec2(static_cast<float>(FontSize) / static_cast<float>(FontLoaderTTF::GetSingleton().GetFontQuality())), 
				1.0f));

			glm::vec3 DarkenedColor;
			DarkenedColor.r = std::clamp(Color.r - 0.3f, 0.0f, 1.0f);
			DarkenedColor.g = std::clamp(Color.g - 0.3f, 0.0f, 1.0f);
			DarkenedColor.b = std::clamp(Color.b - 0.3f, 0.0f, 1.0f);

			this->TextStack.push_back({ &CurrentFont, Text, Model, { DarkenedColor, Color.a } });
		}

		glm::mat4 Model;
		Model = glm::translate(Model, glm::vec3(Pos, 0.0f));
		Model = glm::scale(Model, glm::vec3(
			glm::vec2(static_cast<float>(FontSize) / static_cast<float>(FontLoaderTTF::GetSingleton().GetFontQuality())), 1.0f));

		this->TextStack.push_back({ &CurrentFont, Text, Model, Color });
	}
}

void GraphicsRenderer::RenderQuad(const Rect& Source, const Rect& Destination, const Texture& QuadTexture, 
	double RotationAngle, bool ScreenSpace, bool Emissive, float BrightnessThreshold, float OpacityMultiplier) const
{
	// Construct the model matrix for the quad
	glm::dmat4 Model;
	Model = glm::translate(Model, glm::dvec3(Destination.x + (Destination.w / 2.0), Destination.y + (Destination.h / 2.0), 
		0.0));
	Model = glm::scale(Model, glm::dvec3(Destination.w, Destination.h, 1.0));
	Model = glm::rotate(Model, glm::radians(RotationAngle), glm::dvec3(0.0, 0.0, 1.0));

	// Next check whether another quad in the stack is indentical with the new quad
	bool AppendedInstanced = false;
	for (auto& QuadObj : this->QuadStack)
	{
		if (QuadObj.DiffuseMap)
		{
			if (QuadObj.DiffuseMap->FilePath == QuadTexture.FilePath && QuadObj.ScreenSpace == ScreenSpace &&
				QuadObj.TextureAtlas == Source && QuadObj.BrightnessThreshold == BrightnessThreshold &&
				QuadObj.OpacityMultiplier == OpacityMultiplier)
			{
				QuadObj.ModelMatrices.emplace_back(Model);
				AppendedInstanced = true;
				break;
			}
		}
	}

	// It hasnt been appended to an quad instancing array therefore create a new quad render object
	if (!AppendedInstanced)
	{
		std::vector<glm::mat4> ModelMatrices = { Model };
		this->QuadStack.push_back({ &QuadTexture, Source, glm::vec4(0.0f), ModelMatrices, ScreenSpace, Emissive, 
			BrightnessThreshold, OpacityMultiplier});
	}
}

void GraphicsRenderer::RenderFillQuad(const Rect& Destination, const glm::vec4& QuadColor, double RotationAngle, 
	bool ScreenSpace, bool Emissive) const
{
	// Construct the model matrix for the quad
	glm::dmat4 Model;
	Model = glm::translate(Model, glm::dvec3(Destination.x + (Destination.w / 2.0), Destination.y + (Destination.h / 2.0),
		0.0));
	Model = glm::scale(Model, glm::dvec3(Destination.w, Destination.h, 1.0));
	Model = glm::rotate(Model, glm::radians(RotationAngle), glm::dvec3(0.0, 0.0, 1.0));

	// Next check whether another quad in the stack is indentical with the new quad
	bool AppendedInstanced = false;
	for (auto& QuadObj : this->QuadStack)
	{
		if (QuadObj.DiffuseMap == nullptr && QuadObj.ScreenSpace == ScreenSpace && QuadObj.Color == QuadColor &&
			QuadObj.TextureAtlas == Rect(0, 0, 0, 0) && QuadObj.BrightnessThreshold == 1.0f &&
			QuadObj.OpacityMultiplier == 1.0f)
		{
			QuadObj.ModelMatrices.emplace_back(Model);
			AppendedInstanced = true;
			break;
		}
	}

	// It hasnt been appended to an quad instancing array therefore create a new quad render object
	if (!AppendedInstanced)
	{
		std::vector<glm::mat4> ModelMatrices = { Model };
		this->QuadStack.push_back({ nullptr, Rect(0, 0, 0, 0), QuadColor, ModelMatrices, ScreenSpace, Emissive, 1.0f, 1.0f });
	}
}


void GraphicsRenderer::RefreshScreen(const FrameBufferPtr& RenderingFBO) const
{
	// Bind the framebuffer given so it can be cleared (else if NULLPTR clear the default framebuffer)
	RenderingFBO ? RenderingFBO->BindBuffer() : glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(this->ClearColor.r, this->ClearColor.g, this->ClearColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsRenderer::FlushStack(const OrthoCamera& Camera, const FrameBufferPtr& RenderingFBO, int ResolutionWidth,
	int ResolutionHeight) const
{
	// Bind the framebuffer given so it can be drawn to (else if NULLPTR draw to the default framebuffer)
	RenderingFBO ? RenderingFBO->BindBuffer() : glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, ResolutionWidth, ResolutionHeight);
	
	// Render the quad objects first in the quad stack
	for (const auto& QuadObj : this->QuadStack)
	{
		this->BindShaderProgram(*this->QuadShader); // Bind the shaders to use when rendering object

		// Setup the shader uniforms
		if (QuadObj.ScreenSpace)
			this->QuadShader->SetUniformEx<glm::mat4>("CameraMatrix", Camera.GetProjectionMatrix());
		else
			this->QuadShader->SetUniformEx<glm::mat4>("CameraMatrix", Camera.GetProjectionMatrix() * Camera.GetViewMatrix());

		// Calculate UV coords based on texture atlas data
		if (QuadObj.DiffuseMap)
		{
			constexpr float TEXEL_OFFSET = 1.0f;
			const glm::vec2 TEXTURE_SIZE = glm::vec2(static_cast<float>(QuadObj.DiffuseMap->TextureBuffer->GetWidth()),
				static_cast<float>(QuadObj.DiffuseMap->TextureBuffer->GetHeight()));

			const auto& ATLAS = QuadObj.TextureAtlas;
			std::array<float, 8> UVCoords
			{
				((float)ATLAS.x + TEXEL_OFFSET) / TEXTURE_SIZE.x, ((float)ATLAS.y + TEXEL_OFFSET) / TEXTURE_SIZE.y,
				((float)ATLAS.x + (float)ATLAS.w - TEXEL_OFFSET) / TEXTURE_SIZE.x, ((float)ATLAS.y + TEXEL_OFFSET) / TEXTURE_SIZE.y,
				((float)ATLAS.x + TEXEL_OFFSET) / TEXTURE_SIZE.x, ((float)ATLAS.y + (float)ATLAS.h - TEXEL_OFFSET) / TEXTURE_SIZE.y,
				((float)ATLAS.x + (float)ATLAS.w - TEXEL_OFFSET) / TEXTURE_SIZE.x, ((float)ATLAS.y + (float)ATLAS.h - TEXEL_OFFSET) / TEXTURE_SIZE.y,
			};

			this->QuadVbo->UpdateBufferData(UVCoords.data(), 8 * sizeof(float), 8 * sizeof(float));
		}

		// Generate the instancing matrix buffer for the models given by quad render data
		auto InstancedArray = Buffer::GenerateVertexBuffer(&QuadObj.ModelMatrices[0],
			static_cast<uint32_t>(QuadObj.ModelMatrices.size() * sizeof(glm::mat4)), GL_STATIC_DRAW);

		this->QuadVao->PushLayout<float>(2, 4, sizeof(glm::mat4), 0, 1);
		this->QuadVao->PushLayout<float>(3, 4, sizeof(glm::mat4), sizeof(glm::vec4), 1);
		this->QuadVao->PushLayout<float>(4, 4, sizeof(glm::mat4), 2 * sizeof(glm::vec4), 1);
		this->QuadVao->PushLayout<float>(5, 4, sizeof(glm::mat4), 3 * sizeof(glm::vec4), 1);
		this->QuadVao->AttachBuffers(InstancedArray);

		// Now we finally draw the quad (or quads if multiple instances)
		this->QuadShader->SetUniform<bool>("Mat.Emissive", QuadObj.Emissive);
		this->QuadShader->SetUniform<bool>("Mat.UseTextures", QuadObj.DiffuseMap);

		if (QuadObj.DiffuseMap)
		{
			this->QuadShader->SetUniform<float>("Mat.BrightnessThreshold", QuadObj.BrightnessThreshold);
			this->QuadShader->SetUniform<float>("Mat.OpacityMultiplier", QuadObj.OpacityMultiplier);
			this->QuadShader->SetUniform<int>("Mat.DiffuseMap", 0);

			QuadObj.DiffuseMap->TextureBuffer->BindTexture(0);
		}
		else
			this->QuadShader->SetUniformEx<glm::vec4>("Mat.DiffuseColor", QuadObj.Color);

		this->QuadVao->BindVertexArray();
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(QuadObj.ModelMatrices.size()));
	}

	// Now render the text objects in text stack
	for (const auto& TextObj : this->TextStack)
	{
		this->BindShaderProgram(*this->TextShader); // Bind the shaders to use when rendering object

		// Generate the VBO, IBO and VAO for the text object
		const BatchedData DATA = this->GenerateBatchTextData(TextObj.String, *TextObj.CurrentFont);
		auto TextVBO = Buffer::GenerateVertexBuffer(&DATA.VertexData[0],
			static_cast<uint32_t>(DATA.VertexData.size() * sizeof(float)), GL_STATIC_DRAW);
		auto TextIBO = Buffer::GenerateIndexBuffer(&DATA.IndexData[0],
			static_cast<uint32_t>(DATA.IndexData.size() * sizeof(uint32_t)), GL_STATIC_DRAW);
		auto TextVAO = Buffer::GenerateVertexArray();

		TextVAO->PushLayout<float>(0, 2, 4 * sizeof(float));
		TextVAO->PushLayout<float>(1, 2, 4 * sizeof(float), 2 * sizeof(float));
		TextVAO->AttachBuffers(TextVBO, TextIBO);

		// Bind the shader uniforms
		this->TextShader->SetUniformEx<glm::mat4>("Model", TextObj.ModelMatrix);
		this->TextShader->SetUniformEx<glm::mat4>("Projection", Camera.GetProjectionMatrix());
		this->TextShader->SetUniformEx<glm::vec4>("TextColor", TextObj.Color);
		this->TextShader->SetUniform<int>("FontBitmap", 0);

		// Finally actually render the text
		TextObj.CurrentFont->FontBitmap->BindTexture(0);
		TextVAO->BindVertexArray();

		glDrawElements(GL_TRIANGLES, static_cast<int>(DATA.IndexData.size()), GL_UNSIGNED_INT, nullptr);
	}

	this->QuadStack.clear();
	this->TextStack.clear();
}

GraphicsRenderer& GraphicsRenderer::GetSingleton()
{
	static GraphicsRenderer Singleton;
	return Singleton;
}

glm::vec2 GraphicsRenderer::GetTextSize(const std::string& Text, const Font& CurrentFont, uint32_t FontSize) const
{
	glm::vec2 TotalSize;
	float MinY = 0.0f, MaxY = 0.0f;
	bool FirstCharacter = true;

	for (uint32_t i = 0; i < Text.size(); i++)
	{
		const Character& CHAR_METRICS = CurrentFont.Metrics[Text[i]];
		
		// WIDTH
		if (FirstCharacter)
		{
			TotalSize.x += (CHAR_METRICS.AdvanceX >> 6);
			FirstCharacter = false;
		}
		else if (i == Text.size() - 1)
			TotalSize.x += CHAR_METRICS.Bearing.x + CHAR_METRICS.Size.x;
		else
			TotalSize.x += CHAR_METRICS.Bearing.x + (CHAR_METRICS.AdvanceX >> 6);

		// MAX POINTS
		if (CHAR_METRICS.Bearing.y > MaxY)
			MaxY = CHAR_METRICS.Bearing.y;

		// MIN POINTS
		if (CHAR_METRICS.Bearing.y - CHAR_METRICS.Size.y < MinY)
			MinY = CHAR_METRICS.Bearing.y - CHAR_METRICS.Size.y;
	}

	TotalSize.y = MaxY - MinY;
	TotalSize *= glm::vec2(static_cast<float>(FontSize) / static_cast<float>(FontLoaderTTF::GetSingleton().GetFontQuality()));
	
	return TotalSize;
}