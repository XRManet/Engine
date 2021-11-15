#pragma once

#include <XRFrameworkBase/XRRenderCommon.h>
#include <XRFrameworkBase/XRCommandBuffer.h>

class XRModel;

struct XRCommandGL;
class XRCommandMemoryPoolGL;

class XRCommandBufferGL : public XRCommandBuffer
{
public:
	XRCommandBufferGL();
	virtual ~XRCommandBufferGL();

public:
	void begin() override;
	void end() override;

	void pushResourceLayout(XRBindPoint bindPoint, XRResourceLayout* resourceLayout) override;
	void popResourceLayout() override;

	void beginPass(XRRenderPassBase* renderPass, XRBeginPassInfo& beginPassInfo) override;
	void endPass() override;

	void bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline) override;

	void bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer) override;

	//void addBarrier() override;

	void draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count) override;
	void drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t start, uint32_t count) override;
	void drawModel(XRPrimitiveTopology topology, XRModel const* model) override;
	//void drawInstanced() override;
	//void drawIndexedInstanced() override;
	//void drawIndirect() override;

	//void dispatch() override;
	
	void executeCommands() override;
	
private:
	XRCommandMemoryPoolGL* _commandMemoryPool;
};

class XRCommandMemoryPoolGL
{
	std::vector<XRCommandGL*> _commands;
	std::vector<uint64_t> _commandLocations;
	std::vector<uint8_t> _commandMemory;

public:
	void ready(uint32_t reserveMemorySize);
	void validateCommands();

public:
	template<typename CommandGL, typename ...Args>
	void emplaceCommand(Args&&... args);

public:
	const std::vector<XRCommandGL*>& getCommandList() const { return _commands; }
};

struct XRCommandGL
{

	XRCommandBuffer::CommandName _cmd;

public:
	XRCommandGL(XRCommandBuffer::CommandName cmd) : _cmd(cmd) {}

	virtual void execute() = 0;
};

struct XRCommandGL_Draw : public XRCommandGL
{
	XRPrimitiveTopology _topology;
	uint32_t _start;
	uint32_t _count;

	XRCommandGL_Draw() : XRCommandGL_Draw(0, 0, 0) {}
	XRCommandGL_Draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count)
		: XRCommandGL(XRCommandBuffer::CommandName::Draw)
		, _topology(topology), _start(start), _count(count)
	{}

	void execute() override;
};

struct XRCommandGL_DrawIndexed : public XRCommandGL
{
	XRPrimitiveTopology _topology;
	XRIndexType _indexType;
	uint32_t _indexStart;
	uint32_t _indexCount;

	XRCommandGL_DrawIndexed() : XRCommandGL_DrawIndexed(0, 0, 0, 0) {}
	XRCommandGL_DrawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount)
		: XRCommandGL(XRCommandBuffer::CommandName::DrawIndexed)
		, _topology(topology), _indexType(indexType), _indexStart(indexStart), _indexCount(indexCount)
	{}

	void execute() override;
};

struct XRCommandGL_DrawModel : public XRCommandGL
{
	XRPrimitiveTopology _topology;
	XRModel const* _model;

	XRCommandGL_DrawModel() : XRCommandGL_DrawModel(0, nullptr) {}
	XRCommandGL_DrawModel(XRPrimitiveTopology topology, XRModel const* model)
		: XRCommandGL(XRCommandBuffer::CommandName::DrawModel)
		, _topology(topology), _model(model)
	{}

	void execute() override;
};

struct XRCommandGL_DrawInstanced : public XRCommandGL
{
	XRCommandGL_DrawInstanced()
		: XRCommandGL(XRCommandBuffer::CommandName::DrawInstanced)
	{}

	void execute() override {}
};

struct XRCommandGL_DrawIndexedInstanced : public XRCommandGL
{
	XRCommandGL_DrawIndexedInstanced()
		: XRCommandGL(XRCommandBuffer::CommandName::DrawIndexedInstanced)
	{}

	void execute() override {}
};

struct XRCommandGL_DrawIndirect : public XRCommandGL
{
	XRCommandGL_DrawIndirect()
		: XRCommandGL(XRCommandBuffer::CommandName::DrawIndirect)
	{}

	void execute() override {}
};

struct XRCommandGL_Dispatch : public XRCommandGL
{
	XRCommandGL_Dispatch()
		: XRCommandGL(XRCommandBuffer::CommandName::Dispatch)
	{}

	void execute() override {}
};

struct XRCommandGL_BeginPass : public XRCommandGL
{
	XRRenderPassBase* _renderPass;
	XRBeginPassInfo _beginPassInfo;

	XRCommandGL_BeginPass()
		: XRCommandGL(XRCommandBuffer::CommandName::BeginPass)
		, _renderPass(nullptr), _beginPassInfo()
	{}

	XRCommandGL_BeginPass(XRRenderPassBase* renderPass, XRBeginPassInfo const& beginPassInfo)
		: XRCommandGL(XRCommandBuffer::CommandName::BeginPass)
		, _renderPass(renderPass), _beginPassInfo(beginPassInfo)
	{}

	void execute() override;
};

struct XRCommandGL_EndPass : public XRCommandGL
{
	XRCommandGL_EndPass()
		: XRCommandGL(XRCommandBuffer::CommandName::EndPass)
	{}

	void execute() override;
};

struct XRCommandGL_AddBarrier : public XRCommandGL
{
	XRCommandGL_AddBarrier()
		: XRCommandGL(XRCommandBuffer::CommandName::AddBarrier)
	{}

	void execute() override;
};