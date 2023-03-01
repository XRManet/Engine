#pragma once

#include <XRFrameworkBase/XRRenderCommon.h>
#include <XRFrameworkBase/XRCommandBuffer.h>

class XRModel;

struct XRCommandVK;
class XRCommandMemoryPoolVK;

class XRCommandBufferVK : public XRCommandBuffer
{
public:
	XRCommandBufferVK();
	virtual ~XRCommandBufferVK();

public:
	void begin() override;
	void end() override;

	void pushResourceLayout(XRBindPoint bindPoint, XRResourceLayout* resourceLayout) override;
	void popResourceLayout() override;

	void beginPass(XRBeginPassInfo& beginPassInfo) override;
	void endPass() override;

	void bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline) override;

	void beginRenderPass(XRBeginRenderPassInfo& beginRenderPassInfo, XRBeginSubPassInfo& beginSubPassInfo) override;
	void nextSubPass(XRBeginSubPassInfo& beginRenderPassInfo) override;
	void endRenderPass() override;

	void bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer) override;

	//void addBarrier() override;

	void draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count) override;
	void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t start, uint32_t count) override;
	void drawModel(XRPrimitiveTopology topology, XRModel const* model) override;
	//void drawInstanced() override;
	//void drawIndexedInstanced() override;
	//void drawIndirect() override;

	//void dispatch() override;
	//void dispatchIndirect() override;
	
	void copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, const void* srcData) override;
	void copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, XRBuffer* srcBuffer, uint32_t srcOffset) override;
	
	void executeCommands() override;
	
private:
	XRCommandMemoryPoolVK* _commandMemoryPool;
};

class XRCommandMemoryPoolVK
{
	std::vector<XRCommandVK*> _commands;
	std::vector<uint64_t> _commandLocations;
	std::vector<uint8_t> _commandMemory;

public:
	void ready(uint32_t reserveMemorySize);
	void validateCommands();

public:
	template<typename CommandGL, typename ...Args>
	void emplaceCommand(Args&&... args);

public:
	const std::vector<XRCommandVK*>& getCommandList() const { return _commands; }
};

struct XRCommandVK
{

	XRCommandBuffer::CommandName _cmd;

public:
	XRCommandVK(XRCommandBuffer::CommandName cmd) : _cmd(cmd) {}

	virtual void execute() = 0;
};

struct XRCommandVK_Draw : public XRCommandVK
{
	XRPrimitiveTopology _topology;
	uint32_t _start;
	uint32_t _count;

	XRCommandVK_Draw() : XRCommandVK_Draw(0, 0, 0) {}
	XRCommandVK_Draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count)
		: XRCommandVK(XRCommandBuffer::CommandName::Draw)
		, _topology(topology), _start(start), _count(count)
	{}

	void execute() override;
};

struct XRCommandVK_DrawIndexed : public XRCommandVK
{
	XRPrimitiveTopology _topology;
	XRIndexType _indexType;
	uint32_t _indexStart;
	uint32_t _indexCount;

	XRCommandVK_DrawIndexed() : XRCommandVK_DrawIndexed(0, 0, 0, 0) {}
	XRCommandVK_DrawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount)
		: XRCommandVK(XRCommandBuffer::CommandName::DrawIndexed)
		, _topology(topology), _indexType(indexType), _indexStart(indexStart), _indexCount(indexCount)
	{}

	void execute() override;
};

struct XRCommandVK_DrawModel : public XRCommandVK
{
	XRPrimitiveTopology _topology;
	XRModel const* _model;

	XRCommandVK_DrawModel() : XRCommandVK_DrawModel(0, nullptr) {}
	XRCommandVK_DrawModel(XRPrimitiveTopology topology, XRModel const* model)
		: XRCommandVK(XRCommandBuffer::CommandName::DrawModel)
		, _topology(topology), _model(model)
	{}

	void execute() override;
};

struct XRCommandVK_DrawInstanced : public XRCommandVK
{
	XRCommandVK_DrawInstanced()
		: XRCommandVK(XRCommandBuffer::CommandName::DrawInstanced)
	{}

	void execute() override {}
};

struct XRCommandVK_DrawIndexedInstanced : public XRCommandVK
{
	XRCommandVK_DrawIndexedInstanced()
		: XRCommandVK(XRCommandBuffer::CommandName::DrawIndexedInstanced)
	{}

	void execute() override {}
};

struct XRCommandVK_DrawIndirect : public XRCommandVK
{
	XRCommandVK_DrawIndirect()
		: XRCommandVK(XRCommandBuffer::CommandName::DrawIndirect)
	{}

	void execute() override {}
};

struct XRCommandVK_Dispatch : public XRCommandVK
{
	XRCommandVK_Dispatch()
		: XRCommandVK(XRCommandBuffer::CommandName::Dispatch)
	{}

	void execute() override {}
};

struct XRCommandVK_BeginPass : public XRCommandVK
{
	XRBeginPassInfo _beginPassInfo;

	XRCommandVK_BeginPass()
		: XRCommandVK(XRCommandBuffer::CommandName::BeginPass)
		, _beginPassInfo()
	{}

	XRCommandVK_BeginPass(XRBeginPassInfo const& beginPassInfo)
		: XRCommandVK(XRCommandBuffer::CommandName::BeginPass)
		, _beginPassInfo(beginPassInfo)
	{}

	void execute() override;
};

struct XRCommandVK_EndPass : public XRCommandVK
{
	XRCommandVK_EndPass()
		: XRCommandVK(XRCommandBuffer::CommandName::EndPass)
	{}

	void execute() override;
};

struct XRCommandVK_BeginRenderPass : public XRCommandVK
{
	XRBeginRenderPassInfo _beginRenderPassInfo;
	XRBeginSubPassInfo _beginSubPassInfo;
	XRClearValue _clearValues[8];

	XRCommandVK_BeginRenderPass()
		: XRCommandVK(XRCommandBuffer::CommandName::BeginRenderPass)
		, _beginRenderPassInfo(), _beginSubPassInfo(), _clearValues()
	{}

	XRCommandVK_BeginRenderPass(XRBeginRenderPassInfo const& renderPass, XRBeginSubPassInfo const& beginPassInfo)
		: XRCommandVK(XRCommandBuffer::CommandName::BeginRenderPass)
		, _beginRenderPassInfo(renderPass), _beginSubPassInfo(beginPassInfo)
	{
		for (uint32_t i = 0; i < _beginRenderPassInfo._clearValues._size; ++i)
		{
			_clearValues[i] = _beginRenderPassInfo._clearValues._data[i];
		}
		_beginRenderPassInfo._clearValues._data = _clearValues;
	}

	void execute() override;
};

struct XRCommandVK_NextSubPass : public XRCommandVK
{
	XRBeginSubPassInfo _beginSubPassInfo;

	XRCommandVK_NextSubPass()
		: XRCommandVK(XRCommandBuffer::CommandName::NextSubPass)
	{}

	XRCommandVK_NextSubPass(XRBeginSubPassInfo const& beginPassInfo)
		: XRCommandVK(XRCommandBuffer::CommandName::NextSubPass)
		, _beginSubPassInfo(beginPassInfo) {}

	void execute() override;
};

struct XRCommandVK_EndRenderPass : public XRCommandVK
{
	XRCommandVK_EndRenderPass()
		: XRCommandVK(XRCommandBuffer::CommandName::EndRenderPass)
	{}

	void execute() override;
};

struct XRCommandVK_AddBarrier : public XRCommandVK
{
	XRCommandVK_AddBarrier()
		: XRCommandVK(XRCommandBuffer::CommandName::AddBarrier)
	{}

	void execute() override;
};

struct XRCommandVK_CopyBuffer : public XRCommandVK
{
	XRBuffer* _dstBuffer;
	uint32_t _dstOffset;
	uint32_t _copyDataSize;
	union {
		const void* _srcData;
		XRBuffer* _srcBuffer;
	};
	uint32_t _srcOffset;

	inline bool isUploading() const { return _srcOffset == ~0; }

	XRCommandVK_CopyBuffer()
		: XRCommandVK_CopyBuffer(nullptr, 0, 0, nullptr, 0)
	{}

	XRCommandVK_CopyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, const void* srcData)
		: XRCommandVK(XRCommandBuffer::CommandName::CopyBuffer)
		, _dstBuffer(dstBuffer), _dstOffset(dstOffset), _copyDataSize(copyDataSize), _srcData(srcData), _srcOffset(~0)
	{}

	XRCommandVK_CopyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, XRBuffer* srcBuffer, uint32_t srcOffset)
		: XRCommandVK(XRCommandBuffer::CommandName::CopyBuffer)
		, _dstBuffer(dstBuffer), _dstOffset(dstOffset), _copyDataSize(copyDataSize), _srcBuffer(srcBuffer), _srcOffset(srcOffset)
	{}

	void execute() override;
};