#pragma once

#include "stdafx.h"

struct XRPipelineDescriptor
{
	char const* _vertexFilename;
	char const* _fragmentFilename;
};

class XRBaseExport XRPipeline
{
public:
	XRPipeline(XRPipelineDescriptor const* descriptor) {}
	virtual ~XRPipeline() {}


public:
	virtual void bind() {}
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRPipeline* xrCreatePipeline(XRPipelineDescriptor const* descriptor);
#else
extern XRPipeline* (*xrCreatePipeline)(XRPipelineDescriptor const* descriptor);
#endif