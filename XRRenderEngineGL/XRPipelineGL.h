#pragma once

#include <XRFrameworkBase/XRPipeline.h>

class XRPipelineGL : public XRPipeline
{
public:
	XRPipelineGL(XRShaderStageDescription const* description);
	virtual ~XRPipelineGL();


public:
	void bind() override;
};