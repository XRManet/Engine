#pragma once

#include <XRFrameworkBase/XRPipeline.h>

class XRPipelineGL : public XRPipeline
{
public:
	XRPipelineGL(XRShaderStageDescription const* description);
	virtual ~XRPipelineGL();


public:
	void bind() override;


private:
	GLuint _glProgram;
	GLuint _glVertexShader;
	GLuint _glFragmentShader;
};