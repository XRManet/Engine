/**
 * @file	XRRenderEngineGL\XRCommandBufferGL.cpp.
 *
 * @brief	XRCommandBufferGL class를 구현합니다.
 * @todo	NV_command_list 확장을 사용할 수 있는 경우와 아닌 경우를 분리해서 구현 해야함
 * 			우선은 해당 확장 사용하지 않고 단순 gl call capture로 구현
 */

#include "stdafx.h"
#include "XRCommandBufferGL.h"
#include "XRBufferGL.h"
#include "XRModelGL.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

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

void XRCommandBufferGL::beginPass(XRRenderPassBase* renderPass, XRBeginPassInfo& beginPassInfo)
{

}

void XRCommandBufferGL::endPass()
{

}

void XRCommandBufferGL::bindPipeline(XRBindPoint bindPoint, XRPipeline* pipeline)
{

}

void XRCommandBufferGL::bindResource(const std::string& bindingName, XRResource<XRBuffer>* buffer)
{
	const GLintptr offset = 0;
	const GLuint resourceUniformBuffer = 0; 

	auto found = _programResources._activeUniformBlocks.find(bindingName);
	if (found != _programResources._activeUniformBlocks.end())
	{
		UniformBlockInfo const& uniformBlockInfo = found->second;
		const GLuint binding = _programResources._indexedUniformBlockBindingInfo[uniformBlockInfo._activeBlockIndex]._binding;
		GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, binding, resourceUniformBuffer, offset, uniformBlockInfo._blockSize));
	}
	else
	{
		// failed to bind
	}
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

	new (&_commandMemory[current]) CommandGL( args... );

	_commandLocations.push_back(required);
}