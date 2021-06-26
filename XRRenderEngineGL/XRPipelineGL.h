#pragma once

#include <XRFrameworkBase/XRPipeline.h>

class XRPipelineGL : public XRPipeline
{
public:
	XRPipelineGL(XRPipelineStateDescription const* description);
	virtual ~XRPipelineGL();


public:
	void bind() override;

public:
	bool buildProgram(XRShaderStageDescription const* shaderStageDescription);

private:
	bool createBindingInformation();


private:
	GLuint _glPipeline;
	GLuint _glProgram;
	GLuint _glVertexShader;
	GLuint _glFragmentShader;

	bool _needUpdate = true;
};