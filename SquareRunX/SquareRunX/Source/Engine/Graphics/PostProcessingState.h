#pragma once
#include "Engine/Buffers/FrameBuffer.h"
#include "Engine/Buffers/VertexArray.h"
#include <glm/glm.hpp>

class ShaderProgram;

class PostProcessing
{
	friend class EngineCore;
private:
	// Framebuffer stuff
	FrameBufferPtr PostProcessFBO;
	Texture2DPtr FBOColorAttachment;

	// Quad buffer stuff
	VertexBufferPtr QuadVBO;
	VertexArrayPtr QuadVAO;

	// Other stuff
	const ShaderProgram* PostProcessShader;

	glm::ivec2 Resolution;
	float Gamma;
	double Opacity;

	int NumSamplesMS;
private:
	PostProcessing();
	~PostProcessing();

	void InitProcess();
public:
	void SetOpacityLevel(double Alpha);
	void RenderProcessedFrame() const;
public:
	static PostProcessing& GetSingleton();

	const FrameBufferPtr& GetFBO() const;
	const double& GetOpacity() const;
	glm::ivec2 GetResolution() const;
};