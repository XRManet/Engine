#pragma once

#include <XRFrameworkBase/XRDefault.h>
#include <XRFrameworkBase/XRPrimitiveTypes.h>
#include <XRFrameworkBase/XRReference.h>
#include <XRFrameworkBase/XRRenderCommon.h>

#include <XRFrameworkBase/RenderEngineChild.h>

class XRCommandBuffer;
class XRRenderDevice;

XRRenderAPI(xrCreateCommandBuffer)(XRRenderDevice* ownerRenderDevice)->XRCommandBuffer*;

enum class XRBindPoint
{
	Graphics,
	Compute,
};

class XRResourceLayout;
class XRWorkPassBase;
class XRRenderPass;
class XRFramebuffer;
class XRPipeline;
class XRPipelineGroup;

template<typename ResourceType>
class XRView;
class XRBuffer;

struct XRBeginPassInfo
{
	XRWorkPassBase* _workPass;
};

struct XRBeginRenderPassInfo
{
	XRRenderPass* _renderPass;
	XRFramebuffer* _framebuffer;
	xr::vec3<uint32_t> _extent;
	xr::ContiguousReference<XRClearValue const> _clearValues;
};

struct XRBeginSubPassInfo
{
	bool _secondaryCommandBuffer;
};

class XRModel;
class XRBaseExport XRCommandBuffer : public xr::RenderDeviceChild
{
public:
	enum CommandName
	{
		// WorkPass 관련
		BeginPass,
		EndPass,

		BindPipeline,

		// RenderPass
		BeginRenderPass,
		NextSubPass,
		EndRenderPass,

		// Barrier 관련
		AddBarrier,

		// Bind 관련
		SetResource,

		// Draw
		Draw,
		DrawIndexed,
		DrawModel,
		DrawInstanced,
		DrawIndexedInstanced,
		DrawIndirect,

		// Dispatch
		Dispatch,
		DispatchIndirect,

		// Copy
		CopyBuffer,

		NumAllCommands,
	};

	enum class CommandBufferState
	{
		Invalid,
		Reset,
		Recording,
		Recorded,
		NumStates,
	};

public:
	XRCommandBuffer(XRRenderDevice* ownerRenderDevice) : xr::RenderDeviceChild(ownerRenderDevice) {}
	virtual ~XRCommandBuffer() {}

public:
	virtual void begin() {}
	virtual void end() {}

	virtual void pushResourceLayout(XRBindPoint bindPoint, XRResourceLayout* resourceLayout) {}
	virtual void popResourceLayout() {}

	virtual void beginPass(XRBeginPassInfo& beginPassInfo) {}
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
	
	virtual void beginRenderPass(XRBeginRenderPassInfo& beginRenderPassInfo, XRBeginSubPassInfo& beginSubPassInfo) {}
	virtual void nextSubPass(XRBeginSubPassInfo& beginRenderPassInfo) {}
	virtual void endRenderPass() {}

	virtual void addBarrier() {}

	/**
	 * @fn	void XRCommandBufferGL::bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer)
	 *
	 * @brief	Bind resource
	 * 			View<Buffer>를 현재 CommandBuffer에 설정된 ResourceLayout에 bind한다. BindPoint에 상관 없이 마지막으로 설정한 ResourceLayout에 bind한다.
	 *
	 * @author	Jiman Jeong
	 * @date	2021-07-03
	 *
	 * @param 		  	bindingName	Name of the binding.
	 * @param [in,out]	buffer	   	If non-null, the buffer.
	 */
	virtual void bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer) {}

	virtual void draw(XRPrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount) {}
	virtual void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount) {}
	virtual void drawModel(XRPrimitiveTopology topology, XRModel const* model) {}
	virtual void drawInstanced() {}
	virtual void drawIndexedInstanced() {}
	virtual void drawIndirect() {}

	virtual void dispatch() {}
	virtual void dispatchIndirect() {}

	virtual void copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, const void* srcData) {}
	virtual void copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, XRBuffer* srcBuffer, uint32_t srcOffset) {}

	// Note: non that state bleeds back of toward the primary command buffer
	virtual void executeCommands() {}

protected:
	XRResourceLayout*	_lastGraphicsResourceLayout;
	XRResourceLayout*	_lastComputeResourceLayout;

	std::vector<XRResourceLayout*> _resourceLayoutStack;
	XRResourceLayout*	_currentResourceLayout;
	XRWorkPassBase*		_currentWorkPass;
	XRPipelineGroup*	_currentPipelineGroup;
	XRPipeline*			_currentPipeline;

	CommandBufferState	_currentState;
};