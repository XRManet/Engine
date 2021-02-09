#include "stdafx.h"
#include "XRCommandBufferGL.h"
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
{

}

XRCommandBufferGL::~XRCommandBufferGL()
{

}

void XRCommandBufferGL::draw(XRPrimitiveTopology topology, uint32_t start, uint32_t count)
{
	GLenum topologyGL = ConvertPrimitiveTopology(topology);
	glDrawArrays(topologyGL, start, count);
}

void XRCommandBufferGL::drawIndexed(XRPrimitiveTopology topology, XRIndexType indexType, uint32_t indexStart, uint32_t indexCount)
{
	GLenum topologyGL = ConvertPrimitiveTopology(topology);
	GLenum indexTypeGL = ConvertIndexType(indexType);

	GL_CALL(glDrawElementsBaseVertex(topologyGL, indexCount, indexTypeGL, (void*)indexStart, 0));
}

void XRCommandBufferGL::drawModel(XRPrimitiveTopology topology, XRModel const* model)
{
	XRModelGL const* modelGL = static_cast<XRModelGL const*>(model);

	GLenum topologyGL = ConvertPrimitiveTopology(topology);
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