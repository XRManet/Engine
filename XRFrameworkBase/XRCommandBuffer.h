#pragma once

#include "stdafx.h"

class XRModel;
class XRBaseExport XRCommandBuffer
{
public:
	XRCommandBuffer() {}
	virtual ~XRCommandBuffer() {}

public:
	virtual void begin() {}
	virtual void end() {}

	virtual void draw(XRPrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount) {}
	virtual void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount) {}
	virtual void drawModel(XRPrimitiveTopology topology, XRModel const* model) {}
	virtual void drawInstanced() {}
	virtual void drawIndexedInstanced() {}
	virtual void drawIndirect() {}

	virtual void dispatch() {}
	
	virtual void addBarrier() {}
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRCommandBuffer* xrCreateCommandBuffer();
#else
extern XRCommandBuffer* (*xrCreateCommandBuffer)();
#endif