#pragma once

#include <XRFrameworkBase/XRRenderCommon.h>
#include <XRFrameworkBase/XRCommandBuffer.h>

class XRModel;
class XRCommandBufferGL : public XRCommandBuffer
{
public:
	XRCommandBufferGL();
	virtual ~XRCommandBufferGL();

public:
	//void begin() override;
	//void end() override;

	void draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count) override;
	void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t start, uint32_t count) override;
	void drawModel(XRPrimitiveTopology topology, XRModel const* model) override;
	//void drawInstanced() override;
	//void drawIndexedInstanced() override;
	//void drawIndirect() override;

	//void dispatch() override;

	//void addBarrier() override;
};