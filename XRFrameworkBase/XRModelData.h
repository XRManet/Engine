#pragma once

#include <stdafx.h>

#include "XRLoadable.h"
#include "XRPrimitiveTypes.h"

// XRModelData
// A default representation data form of XRModel
class XRModelData : public XRLoadable
{
public:
  struct VertexPosition { float x, y, z, w; };
  struct VertexNormal { float x, y, z; };
  struct TextureCoordinate { float u, v, w; };
  struct VertexParameter { float u, v, w; };
  struct FaceElement { int vertex, texture, normal; };
  struct FaceIndexTriangle { int v0, v1, v2; };
  struct FaceIndexQuad { int v0, v1, v2, v3; };
  struct ModelHeader
  {
    size_t vertex_offset = 0;
    size_t index_offset = 0;

    int vertex_count = 0;
    int index_count = 0;

    int primitive_type = 0;
    int vertex_number_in_face = 0;

	int num_vertex_attributes = 0;

    bool IsIndexed() const { return (index_count > 0); }
  };

  struct VertexAttribute
  {
	  size_t offset = 0;
	  uint32_t num_elements = 0;
  };

public:
  XRModelData(std::string && path) : XRLoadable(std::move(path)) { Initialize(); }
  XRModelData(std::string const& path) : XRLoadable(path) { Initialize(); }

private:
  void Initialize();

public:		ModelHeader const*	GetHeader() const	{ return reinterpret_cast<ModelHeader const*>(_memory.data()); }
protected:	ModelHeader *		GetHeader()			{ return reinterpret_cast<ModelHeader*>(_memory.data()); }
};
