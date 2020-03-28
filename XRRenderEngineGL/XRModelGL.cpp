#include "stdafx.h"
#include "XRModelGL.h"

#include <XRFrameworkBase/XRModelData.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

XRInputLayoutGL::XRInputLayoutGL(uint32_t preferredStride, std::vector<XRVertexBufferDesc>&& __inputLayoutDesc)
	: XRInputLayout(preferredStride, std::move(__inputLayoutDesc))
{
	// Parse given model and construct input layout from it.
	// if then, we can get a layout of model and just use it later.
	GL_CALL(glGenVertexArrays(1, &_vao));
	GL_CALL(glBindVertexArray(_vao));

	struct XRVertexAttributeDescGL : XRVertexAttributeDesc { friend XRInputLayoutGL; };
	struct XRVertexBufferDescGL : XRVertexBufferDesc { friend XRInputLayoutGL; };

	auto& inputLayoutDesc = getInputLayoutDesc();
	for (int i = 0; i < inputLayoutDesc.size(); ++i)
	{
		auto& bufferDesc = static_cast<XRVertexBufferDescGL const&>(inputLayoutDesc[i]);
		int numAttributes = bufferDesc.attributes.size();
		for (int j = 0; j < numAttributes; ++j)
		{
			auto& attributeDesc = static_cast<XRVertexAttributeDescGL const&>(bufferDesc.attributes[j]);
			attributeDesc.offset;
		}
	}

	//auto header = model->GetHeader();
	GLuint num = 0;//header->vertex_count;
	GLuint size = 0;
	size_t offset = size * num;
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(offset)));

	//if (header->normal_offset != -1)
	{
		size = sizeof(XRModelData::VertexPosition);
		offset += size * num;
		GL_CALL(glEnableVertexAttribArray(1));
		GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(offset)));
	}

	//if (header->texture_offset != -1)
	{
	}
	GL_CALL(glBindVertexArray(0));
}

XRInputLayoutGL::~XRInputLayoutGL()
{

}

void XRInputLayoutGL::bind() const
{
	glBindVertexArray(_vao);
}

XRModelGL::XRModelGL(XRModelData const* data) : XRModel(data)
{
	const auto* header = _data->GetHeader();
	const GLubyte* address = _data->GetData();
	GL_CALL(glGenBuffers(sizeof(GL) / sizeof(unsigned), GL.vbo));
	
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL.vertex));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * header->vertex_count * 2,
			nullptr, GL_STATIC_DRAW));

		assert(header->vertex_count > 0);
		GLuint offset = 0;
		GLuint size = sizeof(uint32_t) * header->vertex_count;
		GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->vertex_offset));

		//if (header->normal_offset > 0)
		{
			offset = size;
			size += sizeof(XRModelData::VertexNormal) * header->vertex_count;
			//GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->normal_offset));
		}

		//if (header->texture_offset > 0)
		{
			offset = size;
			size += sizeof(XRModelData::TextureCoordinate) * header->vertex_count;
			//GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->texture_offset));
		}
	}

	if (header->IsIndexed())
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index));

		GLuint size = sizeof(XRModelData::FaceIndexTriangle) * header->index_count;
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, address + header->index_offset, GL_STATIC_DRAW));
	}

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	//_inputLayout = new XRInputLayoutGL(data);
}

XRModelGL::~XRModelGL()
{
}

void XRModelGL::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, GL.vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index);
}

uint32_t XRModelGL::getNumVertices() const { return _data->GetHeader()->vertex_count; }
uint32_t XRModelGL::getNumIndices() const { return _data->GetHeader()->index_count; }