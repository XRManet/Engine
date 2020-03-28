#pragma once

#include <stdafx.h>

#include "XRLoadable.h"
#include "XRPrimitiveTypes.h"

#define XR_MODEL_DATA_LAYOUT_AOS	1
#define XR_MODEL_DATA_LAYOUT_SOA	2
#define XR_MODEL_DATA_LAYOUT		XR_MODEL_DATA_LAYOUT_SOA

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

  struct SubmeshHeader
  {
    size_t vertex_offset = 0;
    size_t index_offset = 0;

    uint32_t vertex_count = 0;
    uint32_t index_count = 0;

    bool IsIndexed() const { return (index_count > 0); }
  };

  struct MeshHeader
  {
	  uint32_t numVertices;
	  uint32_t vertexStride;

	  uint32_t numSubmeshes;
	  SubmeshHeader submeshes[0];
  };

  struct ObjectHeader
  {
	  int primitive_type = 0;
	  int vertex_number_in_face = 0;

	  int num_vertex_attributes = 0;

	  uint32_t numMeshes;
	  MeshHeader* meshes[0];
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

public:		ObjectHeader const*	GetHeader() const	{ return reinterpret_cast<ObjectHeader const*>(_memory.data()); }
protected:	ObjectHeader *		GetHeader()			{ return reinterpret_cast<ObjectHeader*>(_memory.data()); }
};

static constexpr uint64_t sizeofMeshHeader = sizeof(XRModelData::ObjectHeader);
