/**
 * @file	XRRenderEngineVK\XRCommandBufferVK.cpp.
 *
 * @brief	XRCommandBufferVK class를 구현합니다.
 * @todo	NV_command_list 확장을 사용할 수 있는 경우와 아닌 경우를 분리해서 구현 해야함
 * 			우선은 해당 확장 사용하지 않고 단순 gl call capture로 구현
 */

#include "stdafx.h"
#include <XRFrameworkBase/XRPipeline.h>

#include "XRCommandBufferVK.h"
#include "XRBufferVK.h"
#include "XRModelVK.h"

namespace
{
static VkPrimitiveTopology ConvertPrimitiveTopology(XRPrimitiveTopology topology);

}

XRCommandBufferVK::XRCommandBufferVK()
	: _commandMemoryPool(nullptr)
{

}

XRCommandBufferVK::~XRCommandBufferVK()
{
	if (nullptr != _commandMemoryPool)
		delete _commandMemoryPool;
}

void XRCommandBufferVK::begin()
{
	if (nullptr == _commandMemoryPool)
		_commandMemoryPool = new XRCommandMemoryPoolVK;

	_commandMemoryPool->ready(1 << 12);

	// Move to common layer
	{
		_resourceLayoutStack.clear();
		_currentResourceLayout = nullptr;
		_resourceLayoutStack.push_back(nullptr);
	}
}

void XRCommandBufferVK::end()
{
	_commandMemoryPool->validateCommands();
}

void XRCommandBufferVK::executeCommands()
{
	uint32_t numCommands = _commandMemoryPool->getCommandList().size();
	for (uint32_t i = 0; i < numCommands; ++i)
	{
		XRCommandVK* const commandGL = _commandMemoryPool->getCommandList()[i];
		commandGL->execute();
	}
}

void XRCommandBufferVK::pushResourceLayout(XRBindPoint bindPoint, XRResourceLayout* resourceLayout)
{
	if (bindPoint == XRBindPoint::Graphics)
		_lastGraphicsResourceLayout = resourceLayout;
	else if (bindPoint == XRBindPoint::Compute)
		_lastComputeResourceLayout = resourceLayout;
	else assert(false);

	_currentResourceLayout = resourceLayout;
	_resourceLayoutStack.push_back(resourceLayout);
}

void XRCommandBufferVK::popResourceLayout()
{
	_resourceLayoutStack.pop_back();
	_currentResourceLayout = _resourceLayoutStack.back();
}

void XRCommandBufferVK::beginPass(XRBeginPassInfo& beginPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_BeginPass>(beginPassInfo);
}

void XRCommandBufferVK::endPass()
{

}

void XRCommandBufferVK::beginRenderPass(XRBeginRenderPassInfo& beginRenderPassInfo, XRBeginSubPassInfo& beginSubPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_BeginRenderPass>(beginRenderPassInfo, beginSubPassInfo);
}

void XRCommandBufferVK::nextSubPass(XRBeginSubPassInfo& beginSubPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_NextSubPass>(beginSubPassInfo);
}

void XRCommandBufferVK::endRenderPass()
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_EndRenderPass>();

}

void XRCommandBufferVK::bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline)
{
	//auto pipelineGL = static_cast<XRPipelineGL*>(pipeline);
	//pipelineGL->bind();
	pipeline->bind();
}

void XRCommandBufferVK::bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer)
{
	auto& viewInfo = buffer->getViewInfo();

	auto bindingInfo = _currentResourceLayout->bindingUniform(bindingName, buffer);
	if (bindingInfo == nullptr)
		return;

	struct XRBufferAccessor : public XRBuffer {
		using XRBuffer::_rhi;
	} *accessor = static_cast<XRBufferAccessor*>(buffer->getResource());

	auto bufferVK = static_cast<XRBufferVK*>(accessor->_rhi);
}

void XRCommandBufferVK::draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_Draw>(topology, start, count);
}

void XRCommandBufferVK::drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_DrawIndexed>(topology, indexType, indexStart, indexCount);
}

void XRCommandBufferVK::drawModel(XRPrimitiveTopology topology, XRModel const* model)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_DrawModel>(topology, model);
}

void XRCommandBufferVK::copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, const void* srcData)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_CopyBuffer>(dstBuffer, dstOffset, copyDataSize, srcData);
}

void XRCommandBufferVK::copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, XRBuffer* srcBuffer, uint32_t srcOffset)
{
	_commandMemoryPool->emplaceCommand<XRCommandVK_CopyBuffer>(dstBuffer, dstOffset, copyDataSize, srcBuffer, srcOffset);
}

void XRCommandVK_Draw::execute()
{
	VkPrimitiveTopology topologyGL = ConvertPrimitiveTopology(_topology);
	vkCmdDraw(VK_NULL_HANDLE, _count, 1, _start, 1);
}

void XRCommandVK_DrawIndexed::execute()
{
	VkPrimitiveTopology topologyGL = ConvertPrimitiveTopology(_topology);

	vkCmdDrawIndexed(VK_NULL_HANDLE, _indexCount, 1, _indexStart, 0, 1);
}

void XRCommandVK_DrawModel::execute()
{
}

void XRCommandVK_BeginPass::execute()
{
}

void XRCommandVK_BeginRenderPass::execute()
{
	auto& attachments = _beginRenderPassInfo._renderPass->getAttachments();

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	//renderPassBeginInfo.renderPass = _beginRenderPassInfo._renderPass;
	//renderPassBeginInfo.framebuffer = _beginRenderPassInfo._framebuffer;

	vkCmdBeginRenderPass(VK_NULL_HANDLE, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void XRCommandVK_NextSubPass::execute()
{
}

void XRCommandVK_EndRenderPass::execute()
{
}

void XRCommandVK_CopyBuffer::execute()
{
	class XRBufferInternal : public XRBuffer
	{
	public:
		inline auto getBufferVK() const { return static_cast<XRBufferVK*>(_rhi); }
	};

	auto dstBufferVK = static_cast<XRBufferInternal*>(_dstBuffer)->getBufferVK();
}

namespace
{
VkPrimitiveTopology ConvertPrimitiveTopology(XRPrimitiveTopology topology)
{
	switch (topology)
	{
	case XRPrimitiveTopology::PointList:					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case XRPrimitiveTopology::LineList:						return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case XRPrimitiveTopology::TriangleList:					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case XRPrimitiveTopology::LineStrip:					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case XRPrimitiveTopology::LineListWithAdjacency:		return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
	case XRPrimitiveTopology::LineStripWithAdjacency:		return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
	case XRPrimitiveTopology::TriangleStrip:				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case XRPrimitiveTopology::TriangleFan:					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	case XRPrimitiveTopology::TriangleListWithAdjacency:	return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
	case XRPrimitiveTopology::TriangleStripWithAdjacency:	return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
	case XRPrimitiveTopology::PatchList:					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	}
	return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

}

void XRCommandMemoryPoolVK::ready(uint32_t reserveMemorySize)
{
	_commands.reserve(32);
	_commandLocations.reserve(32);
	_commandLocations.clear();
	_commandLocations.push_back(0);

	if (_commandMemory.size() < reserveMemorySize)
		_commandMemory.resize(reserveMemorySize);
}

void XRCommandMemoryPoolVK::validateCommands()
{
	_commands.resize(_commandLocations.size() - 1);

	for (uint32_t i = 0; i < _commands.size(); ++i)
		_commands[i] = reinterpret_cast<XRCommandVK*>(&_commandMemory[_commandLocations[i]]);
}

template<typename CommandGL, typename ...Args>
void XRCommandMemoryPoolVK::emplaceCommand(Args&&... args)
{
	uint64_t const& current = _commandLocations.back();
	uint64_t required = current + sizeof(CommandGL);

	if (required > _commandMemory.size())
	{
		size_t const APPROX_ONE_HALF = _commandMemory.size() * 3 / 2;
		_commandMemory.resize(APPROX_ONE_HALF);
	}

	new (&_commandMemory[current]) CommandGL( std::forward<Args>(args)... );

	_commandLocations.push_back(required);
}