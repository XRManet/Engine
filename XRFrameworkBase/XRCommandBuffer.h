#pragma once

#include "stdafx.h"

enum class XRPrimitiveTopology : uint32_t
{
	Points,
	Lines,
	LineStrip,
	Triangles,
	TriangleStrip,
	TriangleFan,
	LinesWithAdjacency,
	LineStripWithAdjacency,
	TrianglesWithAdjacency,
	TriangleStripWithAdjacency,
	Patches,
};

enum class XRIndexType : uint32_t
{
	Index8,
	Index16,
	Index32,
};

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