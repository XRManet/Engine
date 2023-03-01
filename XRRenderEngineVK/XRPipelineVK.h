#pragma once

#include <XRFrameworkBase/XRPipeline.h>

class XRPipelineVK : public XRPipeline
{
public:
	XRPipelineVK(XRPipelineStateDescription const* description);
	virtual ~XRPipelineVK();


public:
	void bind() override;

private:
	bool createBindingInformation();


private:
	VkPipeline _pipeline;

	bool _needUpdate = true;
};