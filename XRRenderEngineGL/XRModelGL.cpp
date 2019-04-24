#include "stdafx.h"
#include "XRModelGL.h"

#include <XRFrameworkBase/XRModelData.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

XRModelGL::XRModelGL(XRModelData* data) : XRModel(data)
{
  const auto* header = _data->GetHeader();

  glGenBuffers(2, GL.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, GL.vertex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(XRModelData::VertexPosition) * header->vertex_count + sizeof(XRModelData::VertexNormal) * header->normal_count,
    nullptr, GL_STATIC_DRAW);

  GLuint offset = 0;
  GLuint size = sizeof(XRModelData::VertexPosition) * header->vertex_count;
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, header + header->vertex_offset);

  offset = size;
  size = sizeof(XRModelData::VertexNormal) * header->normal_count;
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, header + header->normal_offset);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(XRModelData::VertexPosition) * header->vertex_count));

  glEnableVertexAttribArray(0);
}

XRModelGL::~XRModelGL()
{
}