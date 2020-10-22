#include "PostProcessingState.h"
#include "Engine/Filesystem/CFGConfigLoader.h"
#include "Engine/Graphics/ShaderManager.h"
#include "Engine/Graphics/GraphicsRenderer.h"
#include "Engine/Core/WindowFrame.h"
#include "Engine/Core/GameStateBase.h"

#include <array>
#include <algorithm>

PostProcessing::PostProcessing() :
	Gamma(2.2f), Resolution(WindowFrame::GetSingleton().GetWidth(), WindowFrame::GetSingleton().GetHeight()),
	NumSamplesMS(1), Opacity(1.0f), PostProcessShader(nullptr)
{}

PostProcessing::~PostProcessing() {}

void PostProcessing::InitProcess()
{
	this->PostProcessShader = ShaderManager::GetSingleton().LoadShaders("POSTPROCESS_SHADER", 
		"GameFiles/Shaders/FinalPostProcess.glsl.vsh", "GameFiles/Shaders/FinalPostProcess.glsl.fsh");

	// Get the relevant config settings from file
	CFGConfigLoader::GetSingleton().OpenFile("GraphicsQuality");
	this->Gamma = CFGElement::ToDataValue<float>(*CFGConfigLoader::GetSingleton().QueryElement("Gamma"));
	this->NumSamplesMS = CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("MultisamplingQuality"));
	this->Resolution.x = 
		CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("ResolutionWidth"));
	this->Resolution.y = 
		CFGElement::ToDataValue<int>(*CFGConfigLoader::GetSingleton().QueryElement("ResolutionHeight"));
	CFGConfigLoader::GetSingleton().CloseFile();

	// Setup the vertex data and buffers for the quad
	std::array<float, 24> VertexData
	{
		// Vertex Coord     // UV Coords
		-0.5f, -0.5f,		0.0f, 0.0f,
		 0.5f, -0.5f,		1.0f, 0.0f,
		-0.5f,  0.5f,		0.0f, 1.0f,

		-0.5f,  0.5f,		0.0f, 1.0f,
		 0.5f, -0.5f,		1.0f, 0.0f,
		 0.5f,  0.5f,		1.0f, 1.0f
	};

	this->QuadVBO = Buffer::GenerateVertexBuffer(VertexData.data(), sizeof(VertexData), GL_STATIC_DRAW);
	this->QuadVAO = Buffer::GenerateVertexArray();

	this->QuadVAO->PushLayout<float>(0, 2, 4 * sizeof(float));
	this->QuadVAO->PushLayout<float>(1, 2, 4 * sizeof(float), 2 * sizeof(float));
	this->QuadVAO->AttachBuffers(this->QuadVBO);

	// Now setup the framebuffer
	this->FBOColorAttachment = Buffer::GenerateTexture2D(GL_SRGB, this->Resolution.x, this->Resolution.y, 
		this->NumSamplesMS);

	this->PostProcessFBO = Buffer::GenerateFrameBuffer();
	this->PostProcessFBO->AttachTexture2D(GL_COLOR_ATTACHMENT0, this->FBOColorAttachment);
}

void PostProcessing::SetOpacityLevel(float Alpha) { this->Opacity = std::clamp(Alpha, 0.0f, 1.0f); }

void PostProcessing::RenderProcessedFrame() const
{
	// Clear the default framebuffer
	GraphicsRenderer::GetSingleton().SetClearColor(Transition::SCREEN_COLOR);
	GraphicsRenderer::GetSingleton().RefreshScreen();

	glViewport(0, 0, WindowFrame::GetSingleton().GetWidth(), WindowFrame::GetSingleton().GetHeight());

	// Bind the post processing shader
	GraphicsRenderer::GetSingleton().BindShaderProgram(*this->PostProcessShader);

	// Configure shader uniforms, then bind texture and vao
	this->PostProcessShader->SetUniform<int>("FramebufferTexture", 0);
	this->PostProcessShader->SetUniformEx<glm::vec2>("ResolutionSize", this->Resolution);
	
	this->PostProcessShader->SetUniform<int>("NumSamplesMS", this->NumSamplesMS);
	this->PostProcessShader->SetUniform<float>("Opacity", this->Opacity);
	this->PostProcessShader->SetUniform<float>("Gamma", this->Gamma);
	
	// Draw the quad
	this->FBOColorAttachment->BindTexture(0);
	this->QuadVAO->BindVertexArray();

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

const FrameBufferPtr& PostProcessing::GetFBO() const { return this->PostProcessFBO; }

PostProcessing& PostProcessing::GetSingleton()
{
	static PostProcessing Singleton;
	return Singleton;
}

glm::ivec2 PostProcessing::GetResolution() const { return this->Resolution; }

const float& PostProcessing::GetOpacity() const { return this->Opacity; }