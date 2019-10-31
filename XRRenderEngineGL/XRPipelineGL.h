#pragma once

#include <XRFrameworkBase/XRPipeline.h>

class XRPipelineGL : public XRPipeline
{
public:
	XRPipelineGL(XRPipelineDescriptor const* descriptor);
	virtual ~XRPipelineGL();


public:
	void bind() override;
};