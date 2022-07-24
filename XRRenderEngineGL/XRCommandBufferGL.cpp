/**
 * @file	XRRenderEngineGL\XRCommandBufferGL.cpp.
 *
 * @brief	XRCommandBufferGL class를 구현합니다.
 * @todo	NV_command_list 확장을 사용할 수 있는 경우와 아닌 경우를 분리해서 구현 해야함
 * 			우선은 해당 확장 사용하지 않고 단순 gl call capture로 구현
 */

#include "stdafx.h"
#include <XRFrameworkBase/XRPipeline.h>

#include "XRCommandBufferGL.h"
#include "XRBufferGL.h"
#include "XRModelGL.h"

namespace
{
static GLenum ConvertPrimitiveTopology(XRPrimitiveTopology topology);
static GLenum ConvertIndexType(XRIndexType indexType);

}

XRCommandBufferGL::XRCommandBufferGL()
	: _commandMemoryPool(nullptr)
{

}

XRCommandBufferGL::~XRCommandBufferGL()
{
	if (nullptr != _commandMemoryPool)
		delete _commandMemoryPool;
}

void XRCommandBufferGL::begin()
{
	if (nullptr == _commandMemoryPool)
		_commandMemoryPool = new XRCommandMemoryPoolGL;

	_commandMemoryPool->ready(1 << 12);

	// Move to common layer
	{
		_resourceLayoutStack.clear();
		_currentResourceLayout = nullptr;
		_resourceLayoutStack.push_back(nullptr);
	}
}

void XRCommandBufferGL::end()
{
	_commandMemoryPool->validateCommands();
}

void XRCommandBufferGL::executeCommands()
{
	uint32_t numCommands = _commandMemoryPool->getCommandList().size();
	for (uint32_t i = 0; i < numCommands; ++i)
	{
		XRCommandGL* const commandGL = _commandMemoryPool->getCommandList()[i];
		commandGL->execute();
	}
}

void XRCommandBufferGL::pushResourceLayout(XRBindPoint bindPoint, XRResourceLayout* resourceLayout)
{
	if (bindPoint == XRBindPoint::Graphics)
		_lastGraphicsResourceLayout = resourceLayout;
	else if (bindPoint == XRBindPoint::Compute)
		_lastComputeResourceLayout = resourceLayout;
	else assert(false);

	_currentResourceLayout = resourceLayout;
	_resourceLayoutStack.push_back(resourceLayout);
}

void XRCommandBufferGL::popResourceLayout()
{
	_resourceLayoutStack.pop_back();
	_currentResourceLayout = _resourceLayoutStack.back();
}

void XRCommandBufferGL::beginPass(XRBeginPassInfo& beginPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_BeginPass>(beginPassInfo);
}

void XRCommandBufferGL::endPass()
{

}

void XRCommandBufferGL::beginRenderPass(XRBeginRenderPassInfo& beginRenderPassInfo, XRBeginSubPassInfo& beginSubPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_BeginRenderPass>(beginRenderPassInfo, beginSubPassInfo);
}

void XRCommandBufferGL::nextSubPass(XRBeginSubPassInfo& beginSubPassInfo)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_NextSubPass>(beginSubPassInfo);
}

void XRCommandBufferGL::endRenderPass()
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_EndRenderPass>();

}

void XRCommandBufferGL::bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline)
{
	//auto pipelineGL = static_cast<XRPipelineGL*>(pipeline);
	//pipelineGL->bind();
	pipeline->bind();
}

void XRCommandBufferGL::bindResource(const std::string& bindingName, XRView<XRBuffer>* buffer)
{
	auto& viewInfo = buffer->getViewInfo();

	auto bindingInfo = _currentResourceLayout->bindingUniform(bindingName, buffer);
	if (bindingInfo == nullptr)
		return;

	struct XRBufferAccessor : public XRBuffer {
		using XRBuffer::_rhi;
	} *accessor = static_cast<XRBufferAccessor*>(buffer->getResource());

	auto bufferGL = static_cast<XRBufferGL*>(accessor->_rhi);

	//UniformInfo const*	uniformBlockInfo = bindingInfo->_uniformInfo;
	GLuint const		binding = bindingInfo->_binding;
	GLuint const		bufferId = bufferGL->getBufferId();
	GLintptr const 		offset = viewInfo._offset;
	GLsizeiptr const	size = viewInfo._size;

	GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, bindingInfo->_binding, bufferId, offset, size));
}

void XRCommandBufferGL::draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_Draw>(topology, start, count);
}

void XRCommandBufferGL::drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_DrawIndexed>(topology, indexType, indexStart, indexCount);
}

void XRCommandBufferGL::drawModel(XRPrimitiveTopology topology, XRModel const* model)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_DrawModel>(topology, model);
}

void XRCommandBufferGL::copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, const void* srcData)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_CopyBuffer>(dstBuffer, dstOffset, copyDataSize, srcData);
}

void XRCommandBufferGL::copyBuffer(XRBuffer* dstBuffer, uint32_t dstOffset, uint32_t copyDataSize, XRBuffer* srcBuffer, uint32_t srcOffset)
{
	_commandMemoryPool->emplaceCommand<XRCommandGL_CopyBuffer>(dstBuffer, dstOffset, copyDataSize, srcBuffer, srcOffset);
}

void XRCommandGL_Draw::execute()
{
	GLenum topologyGL = ConvertPrimitiveTopology(_topology);
	glDrawArrays(topologyGL, _start, _count);
}

void XRCommandGL_DrawIndexed::execute()
{
	GLenum topologyGL = ConvertPrimitiveTopology(_topology);
	GLenum indexTypeGL = ConvertIndexType(_indexType);

	GL_CALL(glDrawElementsBaseVertex(topologyGL, _indexCount, indexTypeGL, (void*)_indexStart, 0));
}

void XRCommandGL_DrawModel::execute()
{
	XRModelGL const* modelGL = static_cast<XRModelGL const*>(_model);

	GLenum topologyGL = ConvertPrimitiveTopology(_topology);
	GLenum indexTypeGL = modelGL->getIndexType(0);

	MultiDrawElementsBaseVertexInfo info{};

	for (uint32_t m = 0; m < 2; ++m)
	{
		modelGL->getMultiDrawElementsBaseVertexInfo(info, m);

		if (GLEW_ARB_draw_elements_base_vertex)
		{
			glMultiDrawElementsBaseVertex(topologyGL, info._indexCounts, indexTypeGL, (void**)info._indexByteOffsets, info._drawCount, info._vertexBase);
		}
		else
		{
			for (uint32_t i = 0; i < info._drawCount; ++i)
			{
				GL_CALL(glDrawElementsBaseVertex(topologyGL, info._indexCounts[i], indexTypeGL, (void*)(info._indexByteOffsets[i]), info._vertexBase[i]));
			}
		}
	}
	//glMultiDrawElementsBaseVertex(topologyGL, info._indexCounts, indexTypeGL, (void**)info._indexByteOffsets, info._drawCount, info._vertexBase);

	//const uint32_t sampleSubmeshIndex = 0;
	//glDrawElementsBaseVertex(topologyGL, info._indexCounts[sampleSubmeshIndex], indexTypeGL, (void*)info._indexByteOffsets[sampleSubmeshIndex], 0);

	//glMultiDrawElementsIndirect(topologyGL, indexTypeGL, command, _drawCount, 0);
	//glMultiDrawElementsIndirectCount(topologyGL, indexTypeGL, command, _drawCount, maxDrawCount, 0);

	//if (GLEW_NV_command_list)
	//{
	//	if (GLEW_NV_uniform_buffer_unified_memory)
	//	{
	//		typedef struct {
	//			GLuint		header;
	//			GLushort	index;
	//			GLushort	stage;
	//			GLuint64	address;
	//		} GlUniformAddressCommandNV;

	//		typedef struct {
	//			GLuint	count;
	//			GLuint	instanceCount;
	//			GLuint	firstIndex;
	//			GLint	baseVertex;
	//			GLuint	baseInstance;
	//		} DrawElementsIndirectCommand;

	//		GL_DRAW_INDIRECT_BUFFER; // buffer for indirect command
	//		glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, 0, );
	//	}

	//	glMultiDrawElementsIndirectBindlessNV(topologyGL, indexTypeGL, command, _drawCount, 0, 4);
	//}
}

void XRCommandGL_BeginPass::execute()
{
}

void XRCommandGL_BeginRenderPass::execute()
{
	auto& attachments = _beginRenderPassInfo._renderPass->getAttachments();

	//const int32_t numAttachments = attachments.size();
	//assert(numAttachments == _beginRenderPassInfo._clearValues._size);

	//for (int32_t i = 0; i < numAttachments; ++i)
	//{
	//	auto& attachment = attachments[i];
	//	attachment._attachmentLayout._format;
	//}
	//glClearColor(1, 1, 1, 1);
	//
	//glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, &_beginRenderPassInfo._clearValues._data[0]._color._float._x);
}

void XRCommandGL_NextSubPass::execute()
{
}

void XRCommandGL_EndRenderPass::execute()
{
}

void XRCommandGL_CopyBuffer::execute()
{
	class XRBufferInternal : public XRBuffer
	{
	public:
		inline auto getBufferGL() const { return static_cast<XRBufferGL*>(_rhi); }
	};

	auto dstBufferGL = static_cast<XRBufferInternal*>(_dstBuffer)->getBufferGL();
	if (isUploading())
	{
		glNamedBufferSubData(dstBufferGL->getBufferId(), _dstOffset, _copyDataSize, _srcData);
	}
	else
	{
		auto srcBufferGL = static_cast<XRBufferInternal*>(_srcBuffer)->getBufferGL();
		glCopyNamedBufferSubData(srcBufferGL->getBufferId(), dstBufferGL->getBufferId(), _srcOffset, _dstOffset, _copyDataSize);
	}
}

namespace
{
GLenum ConvertPrimitiveTopology(XRPrimitiveTopology topology)
{
	switch (topology)
	{
	case XRPrimitiveTopology::PointList:					return GL_POINTS;
	case XRPrimitiveTopology::LineList:						return GL_LINES;
	case XRPrimitiveTopology::TriangleList:					return GL_TRIANGLES;
	case XRPrimitiveTopology::LineStrip:					return GL_LINE_STRIP;
	case XRPrimitiveTopology::LineListWithAdjacency:		return GL_LINES_ADJACENCY;
	case XRPrimitiveTopology::LineStripWithAdjacency:		return GL_LINE_STRIP_ADJACENCY;
	case XRPrimitiveTopology::TriangleStrip:				return GL_TRIANGLE_STRIP;
	case XRPrimitiveTopology::TriangleFan:					return GL_TRIANGLE_FAN;
	case XRPrimitiveTopology::TriangleListWithAdjacency:	return GL_TRIANGLES_ADJACENCY;
	case XRPrimitiveTopology::TriangleStripWithAdjacency:	return GL_TRIANGLE_STRIP_ADJACENCY;
	case XRPrimitiveTopology::PatchList:					return GL_PATCHES;
	}
	return GL_NONE;
}

GLenum ConvertIndexType(XRIndexType indexType)
{
	switch (indexType)
	{
	case XRIndexType::Index8: return GL_UNSIGNED_BYTE;
	case XRIndexType::Index16: return GL_UNSIGNED_SHORT;
	case XRIndexType::Index32: return GL_UNSIGNED_INT;
	}

	const bool FAILED_TO_CONVERT_INDEX_TYPE = false;
	assert(FAILED_TO_CONVERT_INDEX_TYPE);
	return GL_NONE;
}

}

void XRCommandMemoryPoolGL::ready(uint32_t reserveMemorySize)
{
	_commands.reserve(32);
	_commandLocations.reserve(32);
	_commandLocations.clear();
	_commandLocations.push_back(0);

	if (_commandMemory.size() < reserveMemorySize)
		_commandMemory.resize(reserveMemorySize);
}

void XRCommandMemoryPoolGL::validateCommands()
{
	_commands.resize(_commandLocations.size() - 1);

	for (uint32_t i = 0; i < _commands.size(); ++i)
		_commands[i] = reinterpret_cast<XRCommandGL*>(&_commandMemory[_commandLocations[i]]);
}

template<typename CommandGL, typename ...Args>
void XRCommandMemoryPoolGL::emplaceCommand(Args&&... args)
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
