#include "stdafx.h"
#include "XRCommandBufferGL.h"

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
	GL_CALL(glDrawElementsBaseVertex(topologyGL, indexCount * 3, indexTypeGL, nullptr, indexStart));
	//glDrawElementsBaseVertex(topologyGL, indexCount * 3, indexTypeGL, nullptr, indexStart);
}

namespace
{
GLenum ConvertPrimitiveTopology(XRPrimitiveTopology topology)
{
	switch (topology)
	{
	case XRPrimitiveTopology::Points:						return GL_POINTS;
	case XRPrimitiveTopology::Lines:						return GL_LINES;
	case XRPrimitiveTopology::Triangles:					return GL_TRIANGLES;
	case XRPrimitiveTopology::LineStrip:					return GL_LINE_STRIP;
	case XRPrimitiveTopology::LinesWithAdjacency:			return GL_LINES_ADJACENCY;
	case XRPrimitiveTopology::LineStripWithAdjacency:		return GL_LINE_STRIP_ADJACENCY;
	case XRPrimitiveTopology::TriangleStrip:				return GL_TRIANGLE_STRIP;
	case XRPrimitiveTopology::TriangleFan:					return GL_TRIANGLE_FAN;
	case XRPrimitiveTopology::TrianglesWithAdjacency:		return GL_TRIANGLES_ADJACENCY;
	case XRPrimitiveTopology::TriangleStripWithAdjacency:	return GL_TRIANGLE_STRIP_ADJACENCY;
	case XRPrimitiveTopology::Patches:						return GL_PATCHES;
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