#pragma once

#include "stdafx.h"
#include <XRFrameworkBase/XRPrimitiveTypes.h>

enum class XRBindPoint
{
	Graphics,
	Compute,
};

class XRRenderPassBase;
class XRFramebuffer;
class XRPipeline;
class XRPipelineGroup;

class XRModel;
class XRBaseExport XRCommandBuffer
{
public:
	XRCommandBuffer() {}
	virtual ~XRCommandBuffer() {}

public:
	virtual void begin() {}
	virtual void end() {}

	virtual void beginPass(XRRenderPassBase* renderPass, xr::vec3<uint32_t>& extent) {}
	virtual void endPass() {}

	/**
	 * @fn	virtual void bindPipeline(XRBindPoint bindPoint, );
	 *
	 * @brief	BindPoint가 Graphics일 때, ColorAttachmentCount가 0 이상이면 이 호출은 반드시 RenderPass의 구간 내에 있어야한다. 그 외의 경우는 상관 없다.
	 *
	 * @author	Jiman Jeong
	 * @date	2020-11-28
	 *
	 * @param 	bindPoint	The bind point.
	 */
	virtual void bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline) {}

	virtual void draw(XRPrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount) {}
	virtual void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount) {}
	virtual void drawModel(XRPrimitiveTopology topology, XRModel const* model) {}
	virtual void drawInstanced() {}
	virtual void drawIndexedInstanced() {}
	virtual void drawIndirect() {}

	virtual void dispatch() {}

	// Note: non that state bleeds back of toward the primary command buffer
	virtual void executeCommands() {}
	
	virtual void addBarrier() {}
};

#ifdef XRRHIOPENGL_EXPORTS
XRRenderExport XRCommandBuffer* xrCreateCommandBuffer();
#else
extern XRCommandBuffer* (*xrCreateCommandBuffer)();
#endif