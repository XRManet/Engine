#include "stdafx.h"
#include "XRModelGL.h"

#include <XRFrameworkBase/XRModelData.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

XRInputLayoutGL::XRInputLayoutGL(XRModelData const* model)
{
	// Parse given model and construct input layout from it.
	// if then, we can get a layout of model and just use it later.
  GL_CALL(glGenVertexArrays(1, &_vao));
  GL_CALL(glBindVertexArray(_vao));

  GLuint size = 0;
  GLuint num = 0;
  size_t offset = size * num;
  GL_CALL(glEnableVertexAttribArray(0));
  GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(offset)));

  size = sizeof(XRModelData::VertexPosition);
  offset += size * num;
  GL_CALL(glEnableVertexAttribArray(1));
  GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(offset)));

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
  GL_CALL(glGenBuffers(2, GL.vbo));

  {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL.vertex));

    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * header->vertex_count * 2,
      nullptr, GL_STATIC_DRAW));

    assert(header->vertex_count > 0);
    GLuint offset = 0;
    GLuint size = sizeof(XRModelData::VertexPosition) * header->vertex_count;
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->vertex_offset));

    if (header->normal_offset > 0)
    {
      offset = size;
      size += sizeof(XRModelData::VertexNormal) * header->vertex_count;
      GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->normal_offset));
    }

    if (header->texture_offset > 0)
    {
      offset = size;
      size += sizeof(XRModelData::TextureCoordinate) * header->vertex_count;
      GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->texture_offset));
    }
  }

  if (header->IsIndexed())
  {
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index));

    GLuint size = sizeof(XRModelData::FaceIndexTriangle) * header->index_count;
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, address + header->index_offset, GL_STATIC_DRAW));
  }
  
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

  _inputLayout = new XRInputLayoutGL(data);
}

XRModelGL::~XRModelGL()
{
}

void XRModelGL::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, GL.vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index);
}